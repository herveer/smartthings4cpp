#include <catch2/catch_test_macros.hpp>

#include "smartthings4cpp/client.h"
#include "smartthings4cpp/device.h"  // Device methods (HasBeenRefreshed, Id, ...) used directly in these tests

#include <chrono>
#include <thread>

using namespace smartthings4cpp;

TEST_CASE("A default Client is unauthenticated with the default base URL", "[client]") {
	Client client(PollingMode::Manual);
	REQUIRE_FALSE(client.isAuthenticated());
	REQUIRE(client.getAccessToken().empty());
	REQUIRE(client.getBaseUrl() == Client::DEFAULT_BASE_URL);
}

TEST_CASE("setAccessToken makes the Client authenticated", "[client]") {
	Client client(PollingMode::Manual);
	client.setAccessToken("my-token");
	REQUIRE(client.isAuthenticated());
	REQUIRE(client.getAccessToken() == "my-token");
}

TEST_CASE("The token constructor configures the access token", "[client]") {
	Client client("ctor-token", PollingMode::Manual);
	REQUIRE(client.isAuthenticated());
	REQUIRE(client.getAccessToken() == "ctor-token");
}

TEST_CASE("setBaseUrl overrides the API base URL", "[client]") {
	Client client(PollingMode::Manual);
	client.setBaseUrl("https://example.test/v1");
	REQUIRE(client.getBaseUrl() == "https://example.test/v1");
}

TEST_CASE("validateAuthentication fails fast without a token (no network)", "[client]") {
	Client client(PollingMode::Manual);
	auto result = client.validateAuthentication();
	REQUIRE_FALSE(result.isSuccess());
	REQUIRE(result.error == ErrorCode::AuthenticationRequired);
}

TEST_CASE("Resource getters return empty without a token (no network)", "[client]") {
	Client client(PollingMode::Manual);
	REQUIRE(client.getDevices().empty());
	REQUIRE(client.getDevice("some-id") == nullptr);
	REQUIRE(client.getLocations().empty());
	REQUIRE(client.getRooms("loc").empty());
}

TEST_CASE("pollNow() with no registered devices is a no-op", "[client][polling]") {
	Client client(PollingMode::Manual);
	client.pollNow();
	SUCCEED("did not crash");
}

TEST_CASE("registerDevice() ignores a null Device", "[client][polling]") {
	Client client(PollingMode::Manual);
	client.registerDevice(nullptr);
	client.pollNow();
	SUCCEED("did not crash");
}

TEST_CASE("registerDevice() lets two Device objects for the same id both be tracked", "[client][polling]") {
	Client client(PollingMode::Manual);

	auto d1 = std::make_shared<Device>("dev-1", &client);
	auto d2 = std::make_shared<Device>("dev-1", &client);
	client.registerDevice(d1);
	client.registerDevice(d2);

	REQUIRE(d1 != d2);
	REQUIRE(d1->Id.Get() == d2->Id.Get());

	// Unauthenticated client: the underlying fetch fails fast for both, but
	// pollNow() must still run its dedup-by-id loop over both without crashing.
	client.pollNow();
	REQUIRE_FALSE(d1->HasBeenRefreshed.Get());
	REQUIRE_FALSE(d2->HasBeenRefreshed.Get());
}

TEST_CASE("A destroyed Device is silently dropped from polling (no crash)", "[client][polling]") {
	Client client(PollingMode::Manual);
	{
		auto device = std::make_shared<Device>("dev-1", &client);
		client.registerDevice(device);
	} // device destroyed here; its weak_ptr entry in the registry now expires

	client.pollNow(); // must not crash touching the expired weak_ptr
	SUCCEED("did not crash");
}

TEST_CASE("setPollingInterval clamps to the minimum", "[client][polling]") {
	Client client(PollingMode::Manual);

	client.setPollingInterval(std::chrono::milliseconds(1));
	REQUIRE(client.getPollingInterval() == Client::MIN_POLLING_INTERVAL);

	client.setPollingInterval(std::chrono::seconds(30));
	REQUIRE(client.getPollingInterval() == std::chrono::milliseconds(30000));
}

TEST_CASE("startPolling()/stopPolling() messageType runs and stops promptly", "[client][polling]") {
	Client client(PollingMode::Manual);
	REQUIRE_FALSE(client.isPolling());

	client.startPolling();
	REQUIRE(client.isPolling());

	std::this_thread::sleep_for(std::chrono::milliseconds(60));

	auto stop_started = std::chrono::steady_clock::now();
	client.stopPolling();
	auto stop_elapsed = std::chrono::steady_clock::now() - stop_started;

	REQUIRE_FALSE(client.isPolling());
	// stopPolling() should interrupt immediately rather than waiting out the
	// (default 10s) interval; generous bound to avoid flakiness on a loaded machine.
	REQUIRE(stop_elapsed < std::chrono::seconds(1));

	// A second stopPolling() is a documented no-op, not an error.
	client.stopPolling();
	REQUIRE_FALSE(client.isPolling());
}

TEST_CASE("Destroying a Client with automatic polling active stops it cleanly", "[client][polling]") {
	{
		Client client; // PollingMode::Automatic (default)
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		// ~Client() runs here - must not crash, hang, or std::terminate.
	}
	SUCCEED("destructor did not hang or crash");
}
