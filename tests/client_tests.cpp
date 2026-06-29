#include <catch2/catch_test_macros.hpp>

#include "smartthings4cpp/client.h"
#include "smartthings4cpp/device.h"  // complete type needed to destroy unique_ptr<Device>

using namespace smartthings4cpp;

TEST_CASE("A default Client is unauthenticated with the default base URL", "[client]") {
	Client client;
	REQUIRE_FALSE(client.isAuthenticated());
	REQUIRE(client.getAccessToken().empty());
	REQUIRE(client.getBaseUrl() == Client::DEFAULT_BASE_URL);
}

TEST_CASE("setAccessToken makes the Client authenticated", "[client]") {
	Client client;
	client.setAccessToken("my-token");
	REQUIRE(client.isAuthenticated());
	REQUIRE(client.getAccessToken() == "my-token");
}

TEST_CASE("The token constructor configures the access token", "[client]") {
	Client client("ctor-token");
	REQUIRE(client.isAuthenticated());
	REQUIRE(client.getAccessToken() == "ctor-token");
}

TEST_CASE("setBaseUrl overrides the API base URL", "[client]") {
	Client client;
	client.setBaseUrl("https://example.test/v1");
	REQUIRE(client.getBaseUrl() == "https://example.test/v1");
}

TEST_CASE("validateAuthentication fails fast without a token (no network)", "[client]") {
	Client client;
	auto result = client.validateAuthentication();
	REQUIRE_FALSE(result.isSuccess());
	REQUIRE(result.error == ErrorCode::AuthenticationRequired);
}

TEST_CASE("Resource getters return empty without a token (no network)", "[client]") {
	Client client;
	REQUIRE(client.getDevices().empty());
	REQUIRE(client.getDevice("some-id") == nullptr);
	REQUIRE(client.getLocations().empty());
	REQUIRE(client.getRooms("loc").empty());
}
