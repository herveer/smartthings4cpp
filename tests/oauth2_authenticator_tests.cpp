#include <catch2/catch_test_macros.hpp>

#include "smartthings4cpp/oauth2/oauth2_authenticator.h"

using namespace smartthings4cpp;
using namespace smartthings4cpp::oauth2;

namespace {
    // Everything in this file must stay network-free, matching the rest of the
    // (offline) test suite: exercise only the code paths that return before
    // OAuth2Authenticator ever calls the token endpoint. The one exception is
    // "reaches the token request" below, which points tokenUrl at a malformed
    // URL so it fails during local URL parsing rather than real network I/O.
    OAuth2Config testConfig() {
        OAuth2Config config;
        config.clientId = "test-client";
        config.clientSecret = "test-secret";
        return config;
    }

    // Pulls the state= value back out of a URL built by buildAuthorizeUrl(),
    // which always appends it last.
    std::string extractState(const std::string& authorize_url) {
        auto pos = authorize_url.find("state=");
        REQUIRE(pos != std::string::npos);
        return authorize_url.substr(pos + std::string("state=").size());
    }
}

TEST_CASE("buildAuthorizeUrl produces a well-formed authorize URL", "[oauth2]") {
    OAuth2Authenticator auth(testConfig());

    auto url = auth.buildAuthorizeUrl("http://localhost:9753/callback");

    REQUIRE(url.rfind("https://api.smartthings.com/oauth/authorize?", 0) == 0);
    REQUIRE(url.find("client_id=test-client") != std::string::npos);
    REQUIRE(url.find("response_type=code") != std::string::npos);
    REQUIRE(url.find("redirect_uri=http%3A%2F%2Flocalhost%3A9753%2Fcallback") != std::string::npos);
    REQUIRE(url.find("scope=r%3Adevices%3A%2A") != std::string::npos);
    REQUIRE_FALSE(extractState(url).empty());
}

TEST_CASE("buildAuthorizeUrl generates a fresh state each call", "[oauth2]") {
    OAuth2Authenticator auth(testConfig());

    auto first = extractState(auth.buildAuthorizeUrl("http://localhost:9753/callback"));
    auto second = extractState(auth.buildAuthorizeUrl("http://localhost:9753/callback"));

    REQUIRE(first != second);
}

TEST_CASE("completeAuthorization fails fast with no clientId/clientSecret (no network)", "[oauth2]") {
    OAuth2Authenticator auth{ OAuth2Config{} };

    auto result = auth.completeAuthorization("http://localhost:9753/callback?code=abc&state=xyz");

    REQUIRE_FALSE(result.isSuccess());
    REQUIRE(result.error == ErrorCode::InvalidParameter);
}

TEST_CASE("completeAuthorization rejects an empty redirect_result (no network)", "[oauth2]") {
    OAuth2Authenticator auth(testConfig());

    auto result = auth.completeAuthorization("");

    REQUIRE_FALSE(result.isSuccess());
    REQUIRE(result.error == ErrorCode::InvalidParameter);
}

TEST_CASE("completeAuthorization surfaces a denied-consent error param (no network)", "[oauth2]") {
    OAuth2Authenticator auth(testConfig());
    auth.buildAuthorizeUrl("http://localhost:9753/callback");

    auto result = auth.completeAuthorization(
        "http://localhost:9753/callback?error=access_denied&state=whatever");

    REQUIRE_FALSE(result.isSuccess());
    REQUIRE(result.error == ErrorCode::InvalidRequest);
}

TEST_CASE("completeAuthorization rejects a redirect with no code (no network)", "[oauth2]") {
    OAuth2Authenticator auth(testConfig());
    auto url = auth.buildAuthorizeUrl("http://localhost:9753/callback");
    auto state = extractState(url);

    auto result = auth.completeAuthorization("http://localhost:9753/callback?state=" + state);

    REQUIRE_FALSE(result.isSuccess());
    REQUIRE(result.error == ErrorCode::InvalidParameter);
}

TEST_CASE("completeAuthorization rejects a mismatched state (no network)", "[oauth2]") {
    OAuth2Authenticator auth(testConfig());
    auth.buildAuthorizeUrl("http://localhost:9753/callback");

    auto result = auth.completeAuthorization(
        "http://localhost:9753/callback?code=abc123&state=not-the-generated-state");

    REQUIRE_FALSE(result.isSuccess());
    REQUIRE(result.error == ErrorCode::InvalidRequest);
}

TEST_CASE("completeAuthorization with valid state reaches the token request", "[oauth2]") {
    // Everything up to and including CSRF-state validation passes here, so this
    // proves that path works without needing a real token endpoint: tokenUrl is
    // deliberately malformed so the request fails during local URL parsing
    // (NetworkError), never a real socket/DNS operation.
    auto config = testConfig();
    config.tokenUrl = "";
    OAuth2Authenticator auth(config);
    auto url = auth.buildAuthorizeUrl("http://localhost:9753/callback");
    auto state = extractState(url);

    auto result = auth.completeAuthorization(
        "http://localhost:9753/callback?code=abc123&state=" + state);

    REQUIRE_FALSE(result.isSuccess());
    REQUIRE(result.error == ErrorCode::NetworkError);
}

TEST_CASE("refresh fails fast with no clientId/clientSecret (no network)", "[oauth2]") {
    OAuth2Authenticator auth{ OAuth2Config{} };

    auto result = auth.refresh("some-refresh-token");

    REQUIRE_FALSE(result.isSuccess());
    REQUIRE(result.error == ErrorCode::InvalidParameter);
}

TEST_CASE("refresh rejects an empty refresh token (no network)", "[oauth2]") {
    OAuth2Authenticator auth(testConfig());

    auto result = auth.refresh("");

    REQUIRE_FALSE(result.isSuccess());
    REQUIRE(result.error == ErrorCode::InvalidParameter);
}

TEST_CASE("OAuth2Token::isExpired reflects issuedAt + expiresIn", "[oauth2]") {
    OAuth2Token token;
    token.expiresIn = std::chrono::hours{ 24 };

    SECTION("freshly issued token is not expired") {
        token.issuedAt = std::chrono::system_clock::now();
        REQUIRE_FALSE(token.isExpired());
    }

    SECTION("token issued 25 hours ago is expired") {
        token.issuedAt = std::chrono::system_clock::now() - std::chrono::hours{ 25 };
        REQUIRE(token.isExpired());
    }

    SECTION("token within the default 60s skew of expiry counts as expired") {
        token.issuedAt = std::chrono::system_clock::now() - (std::chrono::hours{ 24 } - std::chrono::seconds{ 30 });
        REQUIRE(token.isExpired());
    }

    SECTION("skew can be tightened to accept a token closer to its real expiry") {
        token.issuedAt = std::chrono::system_clock::now() - (std::chrono::hours{ 24 } - std::chrono::seconds{ 30 });
        REQUIRE_FALSE(token.isExpired(std::chrono::seconds{ 1 }));
    }
}
