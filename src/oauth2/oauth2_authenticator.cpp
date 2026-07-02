#include "smartthings4cpp/oauth2/oauth2_authenticator.h"
#include "smartthings4cpp/base64.h"
#include "smartthings4cpp/http_client.h"
#include "smartthings4cpp/json_utils.h"

#include <nlohmann/json.hpp>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <random>

namespace smartthings4cpp {
namespace oauth2 {

	namespace {
		constexpr std::chrono::milliseconds REQUEST_TIMEOUT{ 10000 };

		std::string urlEncode(const std::string& value) {
			std::string result;
			result.reserve(value.size());
			for (unsigned char c : value) {
				if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
					result += static_cast<char>(c);
				} else {
					char buf[4];
					std::snprintf(buf, sizeof(buf), "%%%02X", c);
					result += buf;
				}
			}
			return result;
		}

		int hexValue(char c) {
			if (c >= '0' && c <= '9') return c - '0';
			if (c >= 'a' && c <= 'f') return c - 'a' + 10;
			if (c >= 'A' && c <= 'F') return c - 'A' + 10;
			return -1;
		}

		std::string urlDecode(const std::string& value) {
			std::string result;
			result.reserve(value.size());
			for (size_t i = 0; i < value.size(); ++i) {
				if (value[i] == '%' && i + 2 < value.size()) {
					int hi = hexValue(value[i + 1]);
					int lo = hexValue(value[i + 2]);
					if (hi >= 0 && lo >= 0) {
						result += static_cast<char>((hi << 4) | lo);
						i += 2;
						continue;
					}
				}
				result += (value[i] == '+') ? ' ' : value[i];
			}
			return result;
		}

		std::string generateState() {
			static thread_local std::mt19937_64 rng{ std::random_device{}() };
			std::uniform_int_distribution<int> dist(0, 15);
			static const char HEX[] = "0123456789abcdef";

			std::string state;
			state.reserve(32);
			for (int i = 0; i < 32; ++i) {
				state += HEX[dist(rng)];
			}
			return state;
		}

		// Result of parsing whatever the caller captured after the OAuth2 redirect:
		// either a full redirect URL (query string parsed for code/state/error) or
		// a bare authorization code (no query string to parse).
		struct ParsedRedirect {
			std::string code;
			std::string state;
			std::string error;
			bool hasState = false;
		};

		ParsedRedirect parseRedirectResult(const std::string& input) {
			ParsedRedirect result;

			auto question_pos = input.find('?');
			if (question_pos == std::string::npos) {
				result.code = input;
				return result;
			}

			std::string query = input.substr(question_pos + 1);
			auto hash_pos = query.find('#');
			if (hash_pos != std::string::npos) {
				query = query.substr(0, hash_pos);
			}

			size_t pos = 0;
			while (pos < query.size()) {
				auto amp_pos = query.find('&', pos);
				std::string pair = (amp_pos == std::string::npos)
					? query.substr(pos) : query.substr(pos, amp_pos - pos);

				auto eq_pos = pair.find('=');
				if (eq_pos != std::string::npos) {
					std::string key = pair.substr(0, eq_pos);
					std::string value = urlDecode(pair.substr(eq_pos + 1));
					if (key == "code") {
						result.code = value;
					} else if (key == "state") {
						result.state = value;
						result.hasState = true;
					} else if (key == "error") {
						result.error = value;
					}
				}

				if (amp_pos == std::string::npos) {
					break;
				}
				pos = amp_pos + 1;
			}

			return result;
		}
	}

	OAuth2Authenticator::OAuth2Authenticator(OAuth2Config config)
		: _config(std::move(config)) {
	}

	std::string OAuth2Authenticator::buildAuthorizeUrl(const std::string& redirect_uri) {
		_pending_state = generateState();
		_pending_redirect_uri = redirect_uri;

		// OAuth2 (RFC 6749 section 3.3) specifies a space-delimited scope list;
		// SmartThings' docs don't state the separator explicitly.
		std::string scope_param;
		for (size_t i = 0; i < _config.scopes.size(); ++i) {
			if (i > 0) {
				scope_param += ' ';
			}
			scope_param += _config.scopes[i];
		}

		return _config.authorizeUrl
			+ "?client_id=" + urlEncode(_config.clientId)
			+ "&response_type=code"
			+ "&redirect_uri=" + urlEncode(redirect_uri)
			+ "&scope=" + urlEncode(scope_param)
			+ "&state=" + urlEncode(_pending_state);
	}

	Result<OAuth2Token> OAuth2Authenticator::completeAuthorization(const std::string& redirect_result) {
		if (_config.clientId.empty() || _config.clientSecret.empty()) {
			return Result<OAuth2Token>(ErrorCode::InvalidParameter,
				"OAuth2Config is missing clientId/clientSecret");
		}
		if (redirect_result.empty()) {
			return Result<OAuth2Token>(ErrorCode::InvalidParameter, "redirect_result is empty");
		}

		ParsedRedirect parsed = parseRedirectResult(redirect_result);

		if (!parsed.error.empty()) {
			return Result<OAuth2Token>(ErrorCode::InvalidRequest,
				"Authorization was not granted: " + parsed.error);
		}

		if (parsed.code.empty()) {
			return Result<OAuth2Token>(ErrorCode::InvalidParameter,
				"Could not find an authorization code in redirect_result");
		}

		if (parsed.hasState && (_pending_state.empty() || parsed.state != _pending_state)) {
			return Result<OAuth2Token>(ErrorCode::InvalidRequest,
				"state does not match the value generated by buildAuthorizeUrl()"
				" (possible CSRF, stale/reused authorize URL, or buildAuthorizeUrl()"
				" was not called first)");
		}

		std::map<std::string, std::string> params;
		params["grant_type"] = "authorization_code";
		params["code"] = parsed.code;
		params["redirect_uri"] = _pending_redirect_uri;

		auto result = requestToken(params);
		if (result.isSuccess()) {
			// The code and state are single-use; drop the pending state so a
			// second (accidental) call can't be mistaken for a valid retry.
			_pending_state.clear();
		}
		return result;
	}

	Result<OAuth2Token> OAuth2Authenticator::refresh(const std::string& refresh_token) {
		if (_config.clientId.empty() || _config.clientSecret.empty()) {
			return Result<OAuth2Token>(ErrorCode::InvalidParameter,
				"OAuth2Config is missing clientId/clientSecret");
		}
		if (refresh_token.empty()) {
			return Result<OAuth2Token>(ErrorCode::InvalidParameter, "refresh_token is empty");
		}

		std::map<std::string, std::string> params;
		params["grant_type"] = "refresh_token";
		params["refresh_token"] = refresh_token;

		return requestToken(params);
	}

	Result<OAuth2Token> OAuth2Authenticator::requestToken(const std::map<std::string, std::string>& form_params) {
		try {
			std::string body;
			for (const auto& [key, value] : form_params) {
				if (!body.empty()) {
					body += '&';
				}
				body += urlEncode(key) + "=" + urlEncode(value);
			}

			std::map<std::string, std::string> headers;
			headers["Content-Type"] = "application/x-www-form-urlencoded";
			headers["Accept"] = "application/json";
			headers["Authorization"] = "Basic " + base64::encode(_config.clientId + ":" + _config.clientSecret);

			HttpClient client;
			client.setTimeout(REQUEST_TIMEOUT);

			auto response = client.post(_config.tokenUrl, body, headers);

			if (response.isSuccess()) {
				auto json_response = json_utils::parse(response.body);

				OAuth2Token token;
				token.accessToken = json_utils::getValueOr<std::string>(json_response, "access_token", "");
				token.refreshToken = json_utils::getValueOr<std::string>(json_response, "refresh_token", "");
				token.tokenType = json_utils::getValueOr<std::string>(json_response, "token_type", "");
				token.scope = json_utils::getValueOr<std::string>(json_response, "scope", "");
				token.issuedAt = std::chrono::system_clock::now();
				token.expiresIn = std::chrono::seconds{
					json_utils::getValueOr<long long>(json_response, "expires_in", 0) };

				if (token.accessToken.empty()) {
					return Result<OAuth2Token>(ErrorCode::ApiError,
						"Token endpoint response did not contain an access_token");
				}

				return Result<OAuth2Token>(token);
			}

			if (response.status_code == 400 || response.status_code == 401) {
				return Result<OAuth2Token>(ErrorCode::AuthenticationFailed,
					"Token endpoint rejected the request (invalid/expired code, refresh token,"
					" or client credentials): " + response.body);
			}

			if (response.status_code == 429) {
				return Result<OAuth2Token>(ErrorCode::RateLimited,
					"Rate limit exceeded while requesting a token");
			}

			if (response.status_code == 0) {
				return Result<OAuth2Token>(ErrorCode::NetworkError,
					"Failed to reach the SmartThings token endpoint: " + response.error_message);
			}

			return Result<OAuth2Token>(ErrorCode::ApiError,
				"Unexpected status " + std::to_string(response.status_code) + " from the token endpoint");
		}
		catch (const std::exception& e) {
			return Result<OAuth2Token>(ErrorCode::UnknownError,
				std::string("Token request error: ") + e.what());
		}
	}

} // namespace oauth2
} // namespace smartthings4cpp
