// Silence MSVC's std::getenv deprecation warning in this demo.
#define _CRT_SECURE_NO_WARNINGS

/**
 * @file authentication.cpp
 * @brief Minimal authentication example for smartthings4cpp
 *
 * Demonstrates configuring a SmartThings Personal Access Token (PAT) and
 * validating it against the cloud API. The token is read from the
 * SMARTTHINGS_TOKEN environment variable, a local "smartthings_token.txt" file,
 * or an interactive prompt (and saved on success).
 *
 * Generate a PAT at https://account.smartthings.com/tokens. In production you
 * would use the OAuth 2.0 authorization-code flow instead of a PAT (planned for
 * a later iteration) and store tokens in an OS keychain rather than a text file.
 */

#include <smartthings4cpp/smartthings4cpp.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace smartthings4cpp;

namespace {

const std::string TOKEN_FILE = "smartthings_token.txt";

std::string trim(const std::string& s) {
	const auto first = s.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) {
		return "";
	}
	const auto last = s.find_last_not_of(" \t\r\n");
	return s.substr(first, last - first + 1);
}

std::string resolveToken() {
	if (const char* env = std::getenv("SMARTTHINGS_TOKEN")) {
		std::string token = trim(env);
		if (!token.empty()) {
			return token;
		}
	}

	std::ifstream file(TOKEN_FILE);
	if (file.is_open()) {
		std::string token;
		std::getline(file, token);
		token = trim(token);
		if (!token.empty()) {
			return token;
		}
	}

	std::cout << "Enter your SmartThings Personal Access Token: ";
	std::string token;
	std::getline(std::cin, token);
	token = trim(token);
	if (!token.empty()) {
		std::ofstream out(TOKEN_FILE);
		if (out.is_open()) {
			out << token << std::endl;
		}
	}
	return token;
}

} // namespace

int main() {
	std::cout << "========================================\n";
	std::cout << "  smartthings4cpp - Authentication Example\n";
	std::cout << "  Version: " << Version::STRING << "\n";
	std::cout << "========================================\n\n";

	std::string token = resolveToken();
	if (token.empty()) {
		std::cerr << "No access token provided. Aborting.\n";
		return 1;
	}

	Client client;
	client.setAccessToken(token);

	if (!client.isAuthenticated()) {
		std::cerr << "Client reports no token configured. Aborting.\n";
		return 1;
	}

	std::cout << "Validating token with the SmartThings API...\n";
	auto validation = client.validateAuthentication();

	if (validation.isSuccess()) {
		std::cout << "Success! The token is valid and ready to use.\n";
		std::cout << "Run the 'device_listing' example to enumerate your devices.\n";
		return 0;
	}

	std::cerr << "Validation failed: " << validation.error_message << "\n";
	std::cerr << "Error code: " << errorCodeToString(validation.error) << "\n";
	std::cerr << "Generate a fresh PAT at https://account.smartthings.com/tokens.\n";
	return 1;
}
