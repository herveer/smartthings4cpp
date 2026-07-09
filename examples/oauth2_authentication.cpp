// Silence MSVC's std::getenv deprecation warning in this demo.
#define _CRT_SECURE_NO_WARNINGS

/**
 * @file oauth2_authentication.cpp
 * @brief OAuth 2.0 authorization-code flow example for smartthings4cpp
 *
 * Demonstrates the full flow using your own self-registered SmartThings
 * "OAuth-In App" (create one once with the SmartThings CLI: `smartthings
 * apps:create`): build an authorize URL, let the user approve access in their
 * browser, exchange what comes back for a token, and validate it. On later
 * runs, a saved token is reused (and transparently refreshed once expired)
 * instead of prompting again.
 *
 * The library itself never opens a browser or listens for the redirect - it
 * only builds the authorize URL and parses whatever you captured afterward.
 * This example captures it the simplest possible way (paste the redirect URL
 * back into the terminal); a GUI application would instead open a real
 * browser/webview and could capture the redirect automatically, without any
 * change to the library.
 *
 * This is a demo of the OAuth2 flow only: tokens are persisted to a
 * plain-text JSON file (same caveat as the PAT example - use an OS keychain
 * in production), and subscriptions/webhooks are a separate, later iteration.
 */

#include <smartthings4cpp/smartthings4cpp.h>

#include <nlohmann/json.hpp>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

using namespace smartthings4cpp;
using namespace smartthings4cpp::oauth2;

namespace {

const std::string CONFIG_FILE = "oauth2_client_config.txt";
const std::string TOKEN_FILE = "oauth2_token.json";
const std::string DEFAULT_REDIRECT_URI = "https://pantry-seventh-fidelity.ngrok-free.dev/oauth/callback";

std::string trim(const std::string& s) {
	const auto first = s.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) {
		return "";
	}
	const auto last = s.find_last_not_of(" \t\r\n");
	return s.substr(first, last - first + 1);
}

struct ClientCredentials {
	std::string clientId;
	std::string clientSecret;
};

ClientCredentials resolveClientCredentials() {
	if (const char* id_env = std::getenv("SMARTTHINGS_CLIENT_ID")) {
		if (const char* secret_env = std::getenv("SMARTTHINGS_CLIENT_SECRET")) {
			std::string id = trim(id_env);
			std::string secret = trim(secret_env);
			if (!id.empty() && !secret.empty()) {
				return { id, secret };
			}
		}
	}

	std::ifstream file(CONFIG_FILE);
	if (file.is_open()) {
		std::string id, secret;
		std::getline(file, id);
		std::getline(file, secret);
		id = trim(id);
		secret = trim(secret);
		if (!id.empty() && !secret.empty()) {
			return { id, secret };
		}
	}

	std::cout << "Enter your OAuth-In App's Client ID: ";
	std::string id;
	std::getline(std::cin, id);
	id = trim(id);

	std::cout << "Enter your OAuth-In App's Client Secret: ";
	std::string secret;
	std::getline(std::cin, secret);
	secret = trim(secret);

	if (!id.empty() && !secret.empty()) {
		std::ofstream out(CONFIG_FILE);
		if (out.is_open()) {
			out << id << "\n" << secret << "\n";
		}
	}

	return { id, secret };
}

void saveToken(const OAuth2Token& token) {
	nlohmann::json j;
	j["accessToken"] = token.accessToken;
	j["refreshToken"] = token.refreshToken;
	j["tokenType"] = token.tokenType;
	j["scope"] = token.scope;
	j["issuedAtEpochSeconds"] = static_cast<int64_t>(
		std::chrono::duration_cast<std::chrono::seconds>(token.issuedAt.time_since_epoch()).count());
	j["expiresInSeconds"] = static_cast<int64_t>(token.expiresIn.count());

	std::ofstream out(TOKEN_FILE);
	if (out.is_open()) {
		out << j.dump(2);
	}
}

std::optional<OAuth2Token> loadToken() {
	std::ifstream in(TOKEN_FILE);
	if (!in.is_open()) {
		return std::nullopt;
	}

	std::stringstream buffer;
	buffer << in.rdbuf();

	try {
		auto j = nlohmann::json::parse(buffer.str());

		OAuth2Token token;
		token.accessToken = j.value("accessToken", "");
		token.refreshToken = j.value("refreshToken", "");
		token.tokenType = j.value("tokenType", "");
		token.scope = j.value("scope", "");
		token.issuedAt = std::chrono::system_clock::time_point(
			std::chrono::seconds(j.value("issuedAtEpochSeconds", int64_t{ 0 })));
		token.expiresIn = std::chrono::seconds(j.value("expiresInSeconds", int64_t{ 0 }));

		if (token.accessToken.empty() || token.refreshToken.empty()) {
			return std::nullopt;
		}
		return token;
	}
	catch (const std::exception&) {
		return std::nullopt;
	}
}

} // namespace

int main() {
	std::cout << "========================================\n";
	std::cout << "  smartthings4cpp - OAuth2 Authentication Example\n";
	std::cout << "  Version: " << Version::STRING << "\n";
	std::cout << "========================================\n\n";

	std::cout << "This example uses your own self-registered SmartThings OAuth-In App.\n";
	std::cout << "Create one once with the SmartThings CLI: smartthings apps:create\n";
	std::cout << "  - App type:      OAuth-In App\n";
	std::cout << "  - Scopes:        r:devices:* x:devices:* r:locations:*\n";
	std::cout << "  - Redirect URI:  " << DEFAULT_REDIRECT_URI << "\n";
	std::cout << "    (or set SMARTTHINGS_REDIRECT_URI to whatever you registered instead -\n";
	std::cout << "    the SmartThings docs ask for HTTPS, but a plain http://localhost\n";
	std::cout << "    redirect URI has worked for self-registered apps in practice)\n\n";

	auto credentials = resolveClientCredentials();
	if (credentials.clientId.empty() || credentials.clientSecret.empty()) {
		std::cerr << "No client id/secret provided. Aborting.\n";
		return 1;
	}

	std::string redirect_uri = DEFAULT_REDIRECT_URI;
	if (const char* redirect_env = std::getenv("SMARTTHINGS_REDIRECT_URI")) {
		std::string value = trim(redirect_env);
		if (!value.empty()) {
			redirect_uri = value;
		}
	}

	OAuth2Config config;
	config.clientId = credentials.clientId;
	config.clientSecret = credentials.clientSecret;
	OAuth2Authenticator authenticator(config);

	auto token = loadToken();

	if (token && token->isExpired()) {
		std::cout << "Saved token is expired; refreshing...\n";
		auto refreshed = authenticator.refresh(token->refreshToken);
		if (refreshed.isSuccess() && refreshed.hasValue()) {
			token = *refreshed.value;
			saveToken(*token);
			std::cout << "Refreshed successfully.\n";
		} else {
			std::cout << "Refresh failed (" << errorCodeToString(refreshed.error) << "): "
				<< refreshed.error_message << "\n";
			std::cout << "Falling back to interactive authorization.\n";
			token.reset();
		}
	}

	if (!token) {
		auto authorize_url = authenticator.buildAuthorizeUrl(redirect_uri);

		std::cout << "\nOpen this URL in a browser and approve access:\n\n  "
			<< authorize_url << "\n\n";
		std::cout << "Your browser will then try to load " << redirect_uri
			<< " - it's fine if that shows\n";
		std::cout << "a connection error, nothing needs to be listening there. Copy the\n";
		std::cout << "full URL from the address bar (or just the 'code' value) and paste\n";
		std::cout << "it below:\n> ";

		std::string pasted;
		std::getline(std::cin, pasted);
		pasted = trim(pasted);

		auto result = authenticator.completeAuthorization(pasted);
		if (!result.isSuccess() || !result.hasValue()) {
			std::cerr << "Authorization failed (" << errorCodeToString(result.error) << "): "
				<< result.error_message << "\n";
			return 1;
		}

		token = *result.value;
		saveToken(*token);
		std::cout << "Authorization succeeded.\n";
	}

	Client client;
	client.setAccessToken(token->accessToken);

	std::cout << "\nValidating token with the SmartThings API...\n";
	auto validation = client.validateAuthentication();

	if (validation.isSuccess()) {
		std::cout << "Success! The token is valid and ready to use.\n";
		std::cout << "Run the 'device_listing' example to enumerate your devices.\n";
		return 0;
	}

	std::cerr << "Validation failed: " << validation.error_message << "\n";
	std::cerr << "Error code: " << errorCodeToString(validation.error) << "\n";
	return 1;
}
