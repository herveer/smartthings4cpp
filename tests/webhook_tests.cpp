#include <catch2/catch_test_macros.hpp>

#include "smartthings4cpp/client.h"
#include "smartthings4cpp/device.h"
#include "smartthings4cpp/webhook.h"
#include "smartthings4cpp/http_server.h"
#include "smartthings4cpp/storage.h"
#include "smartthings4cpp/capabilities.h"
#include "smartthings4cpp/json_utils.h"
#include "smartthings4cpp/oauth2/oauth2_types.h"

#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <optional>

using namespace smartthings4cpp;

namespace {

	// A switch/switchLevel dimmer on its "main" component, matching the
	// device_tests fixture, so we can dispatch a "switch" event to it.
	const char* kDeviceJson = R"({
	  "deviceId": "11111111-2222-3333-4444-555555555555",
	  "label": "Living Room Lamp",
	  "components": [
	    {
	      "id": "main",
	      "capabilities": [
	        { "id": "switch", "version": 1 },
	        { "id": "switchLevel", "version": 1 }
	      ]
	    }
	  ]
	})";

	// A DEVICE_EVENT messageType turning that device's switch on.
	const char* kSwitchOnEvent = R"({
	  "messageType": "EVENT",
	  "executionId": "exec-1",
	  "eventData": {
	    "authToken": "at",
	    "installedApp": { "installedAppId": "ia-1" },
	    "events": [
	      {
	        "eventType": "DEVICE_EVENT",
	        "deviceEvent": {
	          "deviceId": "11111111-2222-3333-4444-555555555555",
	          "componentId": "main",
	          "capability": "switch",
	          "attribute": "switch",
	          "value": "on",
	          "stateChange": true
	        }
	      }
	    ]
	  }
	})";

	// Everything here stays network-free: OAuth clients are built from these
	// stubs, and any path that would reach the real cloud is cut short first
	// (missing installedAppId, empty pending set, or a malformed tokenUrl that
	// fails during local URL parsing - the oauth2_authenticator_tests trick).
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

	// IHttpServer that never opens a socket; tests trigger the protected
	// forwarding helpers directly to simulate requests arriving.
	class StubHttpServer : public IHttpServer {
	public:
		Result<void> start() override {
			started = true;
			return Result<void>();
		}
		void stop() override { started = false; }
		std::string fullOAuthCallbackUri() override {
			return "http://localhost:9753/oauth/callback";
		}

		void simulateOAuthRedirect(std::string args) { onOAuthReceived(std::move(args)); }
		nlohmann::json simulateWebhook(const nlohmann::json& body) { return onWebhookReceived(body); }

		bool started = false;
	};

	oauth2::OAuth2Config testConfig() {
		oauth2::OAuth2Config config;
		config.clientId = "test-client";
		config.clientSecret = "test-secret";
		return config;
	}

	// The keychain blob Client::persistOAuthToken() writes, for preloading.
	std::string tokenBlob(const std::string& accessToken, const std::string& refreshToken,
		long long expiresInSeconds, const std::string& installedAppId = "") {
		nlohmann::json blob = {
			{ "accessToken", accessToken },
			{ "refreshToken", refreshToken },
			{ "installedAppId", installedAppId },
			{ "scope", "r:devices:*" },
			{ "issuedAtEpochSeconds", std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::system_clock::now().time_since_epoch()).count() },
			{ "expiresInSeconds", expiresInSeconds }
		};
		return blob.dump();
	}

} // namespace

// --- messageType basics (mode-independent, PAT client) ------------------------

TEST_CASE("handleWebhook echoes a PING challenge", "[webhook]") {
	Client client(PollingMode::Manual);

	auto resp = client.handleWebhook(R"({"messageType":"PING","pingData":{"challenge":"CH-123"}})");

	REQUIRE(resp.messageType == WebhookMessageType::Ping);
	REQUIRE(resp.statusCode == 200);
	auto body = json_utils::parse(resp.body);
	REQUIRE(body["pingData"]["challenge"] == "CH-123");
}

TEST_CASE("handleWebhook rejects a non-JSON or messageType-less body", "[webhook]") {
	Client client(PollingMode::Manual);

	REQUIRE(client.handleWebhook("this is not json").statusCode == 400);
	REQUIRE(client.handleWebhook(R"({"noLifecycle":true})").statusCode == 400);
}

TEST_CASE("handleWebhook acknowledges an unknown messageType without erroring", "[webhook]") {
	Client client(PollingMode::Manual);

	auto resp = client.handleWebhook(R"({"messageType":"SOME_FUTURE_PHASE"})");

	REQUIRE(resp.statusCode == 200);
	REQUIRE(resp.messageType == WebhookMessageType::Unknown);
}

TEST_CASE("handleWebhook handles CONFIRMATION without a URL (no network)", "[webhook]") {
	// With no confirmationUrl there is no outbound GET, so this stays offline;
	// the real URL-confirming GET is exercised manually, like OAuth's token call.
	Client client(PollingMode::Manual);

	auto resp = client.handleWebhook(R"({"messageType":"CONFIRMATION","confirmationData":{"appId":"a"}})");

	REQUIRE(resp.messageType == WebhookMessageType::Confirmation);
	REQUIRE(resp.statusCode == 200);
}

// --- event dispatch ----------------------------------------------------------

TEST_CASE("handleWebhook dispatches a device EVENT onto a registered Device", "[webhook]") {
	// Device built with no Client so attribute reads stay offline (a null client
	// makes the lazy refresh a no-op) - we only care about the event dispatch.
	auto device = std::make_shared<Device>();
	device->initFromJson(json_utils::parse(kDeviceJson));

	Client client(PollingMode::Manual);
	client.registerDevice(device);

	int deviceChanges = 0;
	Capability* firedSender = nullptr;
	auto sub = device->PropertyChanged.SubscribeScoped(
		[&](ObservableObject& obj, PropertyChangedArgs) {
			++deviceChanges;
			firedSender = &static_cast<Capability&>(obj);
		});

	auto resp = client.handleWebhook(kSwitchOnEvent);

	REQUIRE(resp.messageType == WebhookMessageType::Event);
	REQUIRE(deviceChanges > 0);

	auto* sw = device->getCapability<standard::Switch>();
	REQUIRE(sw != nullptr);
	REQUIRE(firedSender == sw);
	// The new value flowed through the same reactive path a poll would take.
	REQUIRE(sw->statusJson()["switch"]["value"] == "on");
}

TEST_CASE("handleWebhook fans one event out to every Device sharing the id", "[webhook]") {
	auto deviceA = std::make_shared<Device>();
	deviceA->initFromJson(json_utils::parse(kDeviceJson));
	auto deviceB = std::make_shared<Device>();
	deviceB->initFromJson(json_utils::parse(kDeviceJson));

	Client client(PollingMode::Manual);
	client.registerDevice(deviceA);
	client.registerDevice(deviceB);

	int a = 0, b = 0;
	auto subA = deviceA->PropertyChanged.SubscribeScoped(
		[&](ObservableObject&, PropertyChangedArgs) { ++a; });
	auto subB = deviceB->PropertyChanged.SubscribeScoped(
		[&](ObservableObject&, PropertyChangedArgs) { ++b; });

	client.handleWebhook(kSwitchOnEvent);

	REQUIRE(a > 0);
	REQUIRE(b > 0);
}

// --- subscription plumbing ----------------------------------------------------

TEST_CASE("buildSubscriptionBody produces the SmartThings device-subscription shape", "[webhook]") {
	SubscriptionRequest request;
	request.deviceId = "DEV-1";
	request.capability = "switch";
	request.attribute = "switch";

	auto body = buildSubscriptionBody(request);

	REQUIRE(body["sourceType"] == "DEVICE");
	REQUIRE(body["device"]["deviceId"] == "DEV-1");
	REQUIRE(body["device"]["componentId"] == "main");
	REQUIRE(body["device"]["capability"] == "switch");
	REQUIRE(body["device"]["attribute"] == "switch");
	REQUIRE(body["device"]["stateChangeOnly"] == true);
	REQUIRE(body["device"]["subscriptionName"].is_string());
	REQUIRE_FALSE(body["device"]["subscriptionName"].get<std::string>().empty());
}

TEST_CASE("buildSubscriptionBody defaults capability/attribute to the wildcard", "[webhook]") {
	SubscriptionRequest request;
	request.deviceId = "DEV-1";

	auto body = buildSubscriptionBody(request);

	REQUIRE(body["device"]["capability"] == "*");
	REQUIRE(body["device"]["attribute"] == "*");
}

TEST_CASE("subscription calls require an OAuth-mode Client (no network)", "[webhook]") {
	Client client(PollingMode::Manual); // PAT mode

	SubscriptionRequest request;
	request.deviceId = "DEV-1";

	auto result = client.subscribeTo(request);
	REQUIRE_FALSE(result.isSuccess());
	REQUIRE(result.error == ErrorCode::InvalidRequest);
}

TEST_CASE("subscription calls need an installedAppId before hitting the network", "[webhook]") {
	Client client(testConfig(), std::make_unique<StubHttpServer>(),
		std::make_unique<InMemoryStorage>(), std::make_unique<InMemoryStorage>());

	SubscriptionRequest request;
	request.deviceId = "DEV-1";

	auto result = client.subscribeTo(request);
	REQUIRE_FALSE(result.isSuccess());
	REQUIRE(result.error == ErrorCode::InvalidParameter);
}

// --- installed-app messageType ---------------------------------------------------

TEST_CASE("handleWebhook captures and persists the installedAppId from Install", "[webhook]") {
	auto storage = std::make_unique<InMemoryStorage>();
	auto* storageRaw = storage.get();
	Client client(testConfig(), std::make_unique<StubHttpServer>(),
		std::make_unique<InMemoryStorage>(), std::move(storage));

	const char* install = R"({
	  "messageType": "INSTALL",
	  "installData": {
	    "authToken": "install-token",
	    "installedApp": { "installedAppId": "IA-42", "locationId": "loc" }
	  }
	})";

	auto resp = client.handleWebhook(install);

	REQUIRE(resp.messageType == WebhookMessageType::Install);
	REQUIRE(client.installedAppId() == "IA-42");
	// Persisted so the next run knows it without waiting for a webhook.
	REQUIRE(storageRaw->getValue("installedAppId").value_or("") == "IA-42");
}

TEST_CASE("handleWebhook wipes persisted state on UNINSTALL", "[webhook]") {
	auto keychain = std::make_unique<InMemoryStorage>();
	auto storage = std::make_unique<InMemoryStorage>();
	auto* keychainRaw = keychain.get();
	auto* storageRaw = storage.get();
	keychain->setValue("oauth.token", tokenBlob("access-token", "refresh-token", 86400, "IA-42"));
	storage->setValue("installedAppId", "IA-42");

	Client client(testConfig(), std::make_unique<StubHttpServer>(),
		std::move(keychain), std::move(storage));
	REQUIRE(client.installedAppId() == "IA-42"); // rehydrated at construction
	REQUIRE(client.isAuthenticated());

	auto resp = client.handleWebhook(R"({"messageType":"UNINSTALL","uninstallData":{}})");

	REQUIRE(resp.messageType == WebhookMessageType::Uninstall);
	REQUIRE(client.installedAppId().empty());
	REQUIRE_FALSE(client.isAuthenticated());
	REQUIRE_FALSE(keychainRaw->getValue("oauth.token").has_value());
	REQUIRE_FALSE(storageRaw->getValue("installedAppId").has_value());
}

// --- authenticate() ------------------------------------------------------------

TEST_CASE("authenticate() requires an OAuth-mode Client", "[webhook]") {
	Client client(PollingMode::Manual);

	auto result = client.authenticate();
	REQUIRE_FALSE(result.isSuccess());
	REQUIRE(result.error == ErrorCode::InvalidRequest);
}

TEST_CASE("authenticate() is silent when a valid token is already stored", "[webhook]") {
	auto keychain = std::make_unique<InMemoryStorage>();
	keychain->setValue("oauth.token", tokenBlob("stored-access-token", "refresh", 86400, "IA-7"));

	Client client(testConfig(), std::make_unique<StubHttpServer>(),
		std::move(keychain), std::make_unique<InMemoryStorage>());

	bool browserRequested = false;
	auto browserSub = client.openBrowserRequested.SubscribeScoped(
		[&](std::string) { browserRequested = true; });

	auto result = client.authenticate();

	REQUIRE(result.isSuccess());
	REQUIRE(result.hasValue());
	REQUIRE(*result.value == true);
	REQUIRE_FALSE(browserRequested);
	REQUIRE(client.isAuthenticated());
	REQUIRE(client.getAccessToken() == "stored-access-token");
	REQUIRE(client.installedAppId() == "IA-7");
	REQUIRE(client.waitForAuthentication(std::chrono::milliseconds(1)).isSuccess());
}

TEST_CASE("authenticate() with nothing stored requests the browser step", "[webhook]") {
	Client client(testConfig(), std::make_unique<StubHttpServer>(),
		std::make_unique<InMemoryStorage>(), std::make_unique<InMemoryStorage>());

	std::string url;
	auto browserSub = client.openBrowserRequested.SubscribeScoped(
		[&](std::string u) { url = std::move(u); });

	auto result = client.authenticate();

	REQUIRE(result.isSuccess());
	REQUIRE(result.hasValue());
	REQUIRE(*result.value == false); // pending: browser round-trip required
	REQUIRE(url.find("client_id=test-client") != std::string::npos);
	// redirect_uri comes from the injected server's fullOAuthCallbackUri().
	REQUIRE(url.find("redirect_uri=http%3A%2F%2Flocalhost%3A9753%2Foauth%2Fcallback")
		!= std::string::npos);
	REQUIRE_FALSE(client.isAuthenticated());
}

TEST_CASE("authenticate() surfaces an unreachable token endpoint as an error (no real network)", "[webhook]") {
	// Refresh-token-only + a malformed tokenUrl: the refresh fails during local
	// URL parsing (NetworkError) - consent can't fix that, so it's reported
	// instead of falling through to the browser.
	auto config = testConfig();
	config.tokenUrl = "";
	auto keychain = std::make_unique<InMemoryStorage>();
	keychain->setValue("oauth.token", tokenBlob("", "some-refresh-token", 0));

	Client client(config, std::make_unique<StubHttpServer>(),
		std::move(keychain), std::make_unique<InMemoryStorage>());

	auto result = client.authenticate();

	REQUIRE_FALSE(result.isSuccess());
	REQUIRE(result.error == ErrorCode::NetworkError);
}

TEST_CASE("A bad OAuth redirect completes authentication with an error", "[webhook]") {
	auto server = std::make_unique<StubHttpServer>();
	auto* serverRaw = server.get();
	Client client(testConfig(), std::move(server),
		std::make_unique<InMemoryStorage>(), std::make_unique<InMemoryStorage>());

	auto result = client.authenticate(); // generates the CSRF state
	REQUIRE(result.isSuccess());
	REQUIRE(*result.value == false);

	bool completedFired = false;
	Result<void> outcome;
	auto completedSub = client.authenticationCompleted.SubscribeScoped(
		[&](Result<void> r) {
			completedFired = true;
			outcome = r;
		});

	// The redirect arrives with the wrong state: CSRF rejection, no network.
	serverRaw->simulateOAuthRedirect("/oauth/callback?code=abc&state=not-the-right-state");

	REQUIRE(completedFired);
	REQUIRE_FALSE(outcome.isSuccess());
	REQUIRE(outcome.error == ErrorCode::InvalidRequest);

	auto waited = client.waitForAuthentication(std::chrono::milliseconds(100));
	REQUIRE_FALSE(waited.isSuccess());
	REQUIRE(waited.error == ErrorCode::InvalidRequest);
	REQUIRE_FALSE(client.isAuthenticated());
}
