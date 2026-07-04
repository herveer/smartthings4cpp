#include <catch2/catch_test_macros.hpp>

#include "smartthings4cpp/http_server.h"
#include "smartthings4cpp/http_client.h"
#include "smartthings4cpp/client.h"
#include "smartthings4cpp/device.h"
#include "smartthings4cpp/storage.h"
#include "smartthings4cpp/capabilities.h"
#include "smartthings4cpp/json_utils.h"
#include "smartthings4cpp/oauth2/oauth2_types.h"

#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

using namespace smartthings4cpp;

// These tests open a real socket - on 127.0.0.1 only, with an ephemeral port
// (port 0), so they stay self-contained and never touch the network proper.

namespace {

	const char* kDeviceJson = R"({
	  "deviceId": "11111111-2222-3333-4444-555555555555",
	  "label": "Living Room Lamp",
	  "components": [
	    {
	      "id": "main",
	      "capabilities": [ { "id": "switch", "version": 1 } ]
	    }
	  ]
	})";

	const char* kSwitchOnEvent = R"({
	  "messageType": "EVENT",
	  "eventData": {
	    "events": [
	      {
	        "eventType": "DEVICE_EVENT",
	        "deviceEvent": {
	          "deviceId": "11111111-2222-3333-4444-555555555555",
	          "componentId": "main",
	          "capability": "switch",
	          "attribute": "switch",
	          "value": "on"
	        }
	      }
	    ]
	  }
	})";

	class InMemoryStorage : public IStorageProvider {
	public:
		std::optional<std::string> getValue(const std::string& key) override {
			std::lock_guard<std::mutex> lock(_mutex);
			auto it = _values.find(key);
			if (it == _values.end()) {
				return std::nullopt;
			}
			return it->second;
		}
		bool setValue(const std::string& key, const std::string& value) override {
			std::lock_guard<std::mutex> lock(_mutex);
			_values[key] = value;
			return true;
		}
		void removeValue(const std::string& key) override {
			std::lock_guard<std::mutex> lock(_mutex);
			_values.erase(key);
		}

	private:
		std::mutex _mutex;
		std::map<std::string, std::string> _values;
	};

	// "http://localhost:53211/oauth/callback" -> "http://localhost:53211"
	std::string baseUrlOf(const std::string& fullOAuthUri, const std::string& route) {
		auto pos = fullOAuthUri.find(route);
		REQUIRE(pos != std::string::npos);
		return fullOAuthUri.substr(0, pos);
	}

} // namespace

TEST_CASE("DefaultHttpServer serves the OAuth and webhook routes (loopback)", "[httpserver]") {
	auto server = makeDefaultHttpServer(0, "/oauth/callback", "/webhook");

	std::string oauthArgs;
	nlohmann::json webhookBody;
	server->bind(
		[&](std::string args) { oauthArgs = std::move(args); },
		[&](nlohmann::json body) {
			webhookBody = std::move(body);
			return nlohmann::json{ { "echo", true } };
		});

	REQUIRE(server->start().isSuccess());
	auto oauthUri = server->fullOAuthCallbackUri();
	REQUIRE(oauthUri.rfind("http://localhost:", 0) == 0);
	auto base = baseUrlOf(oauthUri, "/oauth/callback");

	HttpClient http;

	SECTION("GET on the OAuth route forwards the full request target") {
		auto response = http.get(oauthUri + "?code=abc&state=xyz");
		REQUIRE(response.isSuccess());
		REQUIRE(oauthArgs.find("code=abc") != std::string::npos);
		REQUIRE(oauthArgs.find("state=xyz") != std::string::npos);
	}

	SECTION("POST on the webhook route round-trips JSON") {
		auto response = http.post(base + "/webhook", R"({"messageType":"PING"})",
			{ { "Content-Type", "application/json" } });
		REQUIRE(response.isSuccess());
		REQUIRE(webhookBody["messageType"] == "PING");
		REQUIRE(json_utils::parse(response.body)["echo"] == true);
	}

	SECTION("invalid JSON on the webhook route is rejected with 400") {
		auto response = http.post(base + "/webhook", "this is not json",
			{ { "Content-Type", "application/json" } });
		REQUIRE(response.status_code == 400);
		REQUIRE(webhookBody.is_null()); // callback never invoked
	}

	server->stop();
}

TEST_CASE("DefaultHttpServer start() is idempotent and stop() is prompt", "[httpserver]") {
	auto server = makeDefaultHttpServer(0, "/oauth/callback", "/webhook");
	server->bind([](std::string) {}, [](nlohmann::json) { return nlohmann::json::object(); });

	REQUIRE(server->start().isSuccess());
	REQUIRE(server->start().isSuccess()); // second start is a no-op
	server->stop();
	server->stop(); // second stop is a no-op
}

TEST_CASE("A webhook EVENT POSTed to the embedded server updates live Devices", "[httpserver][webhook]") {
	// The full push pipeline, entirely on loopback: real embedded server ->
	// Client::handleWebhook -> device registry fan-out -> Capability update ->
	// PropertyChanged relay. This is what a SmartThings event delivery does,
	// minus the internet.
	auto server = makeDefaultHttpServer(0, "/oauth/callback", "/webhook");
	auto* serverRaw = server.get();

	oauth2::OAuth2Config config;
	config.clientId = "test-client";
	config.clientSecret = "test-secret";
	Client client(config, std::move(server),
		std::make_unique<InMemoryStorage>(), std::make_unique<InMemoryStorage>());

	// Not authenticated: the automatic subscription for this device is parked
	// as pending (no network), but event dispatch works regardless.
	auto device = std::make_shared<Device>();
	device->initFromJson(json_utils::parse(kDeviceJson));
	client.registerDevice(device);

	int changes = 0;
	Capability* firedSender = nullptr;
	auto sub = device->PropertyChanged.SubscribeScoped(
		[&](ObservableObject& obj, PropertyChangedArgs) {
			++changes;
			firedSender = &static_cast<Capability&>(obj);
		});

	auto base = baseUrlOf(serverRaw->fullOAuthCallbackUri(), "/oauth/callback");
	HttpClient http;
	auto response = http.post(base + "/webhook", kSwitchOnEvent,
		{ { "Content-Type", "application/json" } });

	REQUIRE(response.isSuccess());
	REQUIRE(json_utils::parse(response.body).contains("eventData"));
	REQUIRE(changes > 0);

	auto* sw = device->getCapability<Switch>();
	REQUIRE(sw != nullptr);
	REQUIRE(firedSender == sw);
	REQUIRE(sw->statusJson()["switch"]["value"] == "on");
}
