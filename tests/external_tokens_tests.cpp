#include <catch2/catch_test_macros.hpp>

#include "smartthings4cpp/client.h"
#include "smartthings4cpp/device.h"
#include "smartthings4cpp/webhook.h"
#include "smartthings4cpp/http_server.h"
#include "smartthings4cpp/storage.h"
#include "smartthings4cpp/capabilities.h"
#include "smartthings4cpp/json_utils.h"

#include "loopback_http.h"

#include <nlohmann/json.hpp>

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

using namespace smartthings4cpp;

// The external-tokens mode: an OAuth-mode Client whose tokens come from the
// application (setAccessToken) instead of its own OAuth flow. The SmartThings
// API is faked on loopback with the library's own embedded server (one GET +
// one POST route per instance, see loopback_http.h), so nothing leaves the
// machine.

namespace {

	const char* kDeviceId = "11111111-2222-3333-4444-555555555555";

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

	// Storage whose contents outlive the Client that owns the provider, so a
	// test can inspect what was persisted after the Client is gone.
	class SharedStorage : public IStorageProvider {
	public:
		struct State {
			std::mutex mutex;
			std::map<std::string, std::string> values;
		};

		explicit SharedStorage(std::shared_ptr<State> state = std::make_shared<State>())
			: _state(std::move(state)) {
		}

		std::optional<std::string> getValue(const std::string& key) override {
			std::lock_guard<std::mutex> lock(_state->mutex);
			auto it = _state->values.find(key);
			if (it == _state->values.end()) {
				return std::nullopt;
			}
			return it->second;
		}
		bool setValue(const std::string& key, const std::string& value) override {
			std::lock_guard<std::mutex> lock(_state->mutex);
			_state->values[key] = value;
			return true;
		}
		void removeValue(const std::string& key) override {
			std::lock_guard<std::mutex> lock(_state->mutex);
			_state->values.erase(key);
		}

	private:
		std::shared_ptr<State> _state;
	};

	std::optional<std::string> valueIn(const std::shared_ptr<SharedStorage::State>& state,
		const std::string& key) {
		std::lock_guard<std::mutex> lock(state->mutex);
		auto it = state->values.find(key);
		if (it == state->values.end()) {
			return std::nullopt;
		}
		return it->second;
	}

	// IHttpServer that never opens a socket: the test plays the application's
	// gateway and pushes requests in through the protected forwarders.
	class StubHttpServer : public IHttpServer {
	public:
		Result<void> start() override {
			started = true;
			return Result<void>();
		}
		void stop() override { started = false; }
		std::string fullOAuthCallbackUri() override {
			fullOAuthCallbackUriCalled = true;
			return "http://localhost:9753/oauth/callback";
		}

		void simulateOAuthRedirect(std::string args) { onOAuthReceived(std::move(args)); }
		WebhookResponse simulateWebhook(const WebhookRequest& request) { return onWebhookReceived(request); }

		std::atomic<bool> started{ false };
		std::atomic<bool> fullOAuthCallbackUriCalled{ false };
	};

	// A fake SmartThings API on loopback: records every POST on its route and
	// answers it with whatever the test's responder returns (any status).
	class FakeApi {
	public:
		using Responder = std::function<WebhookResponse(const WebhookRequest&)>;

		FakeApi(const std::string& getRoute, const std::string& postRoute) {
			int port = testutil::freeLoopbackPort();
			baseUrl = "http://127.0.0.1:" + std::to_string(port);
			_server = makeDefaultHttpServer(port, getRoute, postRoute, baseUrl);
			_server->bind([](std::string) {},
				[this](const WebhookRequest& request) { return onPost(request); });
			started = _server->start().isSuccess();
		}

		~FakeApi() { _server->stop(); }

		void respondWith(Responder responder) {
			std::lock_guard<std::mutex> lock(_mutex);
			_responder = std::move(responder);
		}

		size_t postCount() const {
			std::lock_guard<std::mutex> lock(_mutex);
			return _posts.size();
		}

		WebhookRequest lastPost() const {
			std::lock_guard<std::mutex> lock(_mutex);
			return _posts.empty() ? WebhookRequest{} : _posts.back();
		}

		std::string baseUrl;
		bool started = false;

	private:
		WebhookResponse onPost(const WebhookRequest& request) {
			Responder responder;
			{
				std::lock_guard<std::mutex> lock(_mutex);
				_posts.push_back(request);
				responder = _responder;
			}
			return responder ? responder(request) : WebhookResponse{};
		}

		std::unique_ptr<IHttpServer> _server;
		mutable std::mutex _mutex;
		std::vector<WebhookRequest> _posts;
		Responder _responder;
	};

	WebhookResponse jsonResponse(int status, std::string body) {
		WebhookResponse response;
		response.statusCode = status;
		response.body = std::move(body);
		return response;
	}

	std::string bearerOf(const WebhookRequest& request) {
		return request.header("Authorization").value_or("");
	}

	// Poll until @p predicate holds (the worker runs asynchronously).
	template <typename Predicate>
	bool waitUntil(Predicate predicate,
		std::chrono::milliseconds timeout = std::chrono::seconds(5)) {
		const auto deadline = std::chrono::steady_clock::now() + timeout;
		while (!predicate()) {
			if (std::chrono::steady_clock::now() > deadline) {
				return false;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
		return true;
	}

	std::shared_ptr<Device> makeLamp() {
		auto device = std::make_shared<Device>(); // no Client: attribute reads stay offline
		device->initFromJson(json_utils::parse(kDeviceJson));
		return device;
	}

	nlohmann::json switchOnCommand() {
		return nlohmann::json{ { "commands", nlohmann::json::array({
			{ { "component", "main" }, { "capability", "switch" }, { "command", "on" } } }) } };
	}

	// Records accessTokenRejected reports (they arrive on the worker thread).
	struct RejectionLog {
		std::mutex mutex;
		std::vector<std::string> tokens;
		std::thread::id thread;

		void record(const std::string& token) {
			std::lock_guard<std::mutex> lock(mutex);
			tokens.push_back(token);
			thread = std::this_thread::get_id();
		}
		size_t count() {
			std::lock_guard<std::mutex> lock(mutex);
			return tokens.size();
		}
	};

} // namespace

TEST_CASE("An external-tokens Client starts signed out and never asks for a browser", "[external-tokens]") {
	auto server = std::make_unique<StubHttpServer>();
	auto* serverRaw = server.get();
	Client client(ExternalTokens{}, std::move(server), std::make_unique<SharedStorage>());

	std::atomic<bool> browserRequested{ false };
	auto browserSub = client.openBrowserRequested.SubscribeScoped(
		[&](std::string) { browserRequested = true; });

	REQUIRE(serverRaw->started);          // bound and started for the webhook
	REQUIRE_FALSE(client.isAuthenticated());
	REQUIRE_FALSE(client.isPolling());    // OAuth mode: no polling by default

	// authenticate(): authenticated iff a token is set - none yet.
	auto authenticated = client.authenticate();
	REQUIRE(authenticated.isSuccess());
	REQUIRE(authenticated.hasValue());
	REQUIRE(*authenticated.value == false);

	// waitForAuthentication() never waits in this mode.
	const auto started = std::chrono::steady_clock::now();
	auto waited = client.waitForAuthentication(std::chrono::hours(1));
	REQUIRE(std::chrono::steady_clock::now() - started < std::chrono::seconds(1));
	REQUIRE(waited.error == ErrorCode::AuthenticationRequired);

	// API calls fail fast with an auth error.
	REQUIRE(client.executeCommands(kDeviceId, switchOnCommand()).error == ErrorCode::AuthenticationRequired);
	REQUIRE(client.validateAuthentication().error == ErrorCode::AuthenticationRequired);

	// The OAuth route is unused: a stray redirect changes nothing.
	serverRaw->simulateOAuthRedirect("/oauth/callback?code=abc&state=xyz");
	REQUIRE_FALSE(client.isAuthenticated());

	REQUIRE_FALSE(browserRequested);
	REQUIRE_FALSE(serverRaw->fullOAuthCallbackUriCalled);
}

TEST_CASE("setAccessToken adopts the token and captures the installed-app id", "[external-tokens]") {
	// Observed state is declared before the Client so it outlives the worker.
	std::atomic<int> completed{ 0 };
	std::atomic<bool> completedOk{ false };
	auto state = std::make_shared<SharedStorage::State>();
	Client client(ExternalTokens{}, std::make_unique<StubHttpServer>(),
		std::make_unique<SharedStorage>(state));

	auto completedSub = client.authenticationCompleted.SubscribeScoped(
		[&](Result<void> outcome) {
			completedOk = outcome.isSuccess();
			++completed;
		});

	client.setAccessToken("token-1", "IA-1");

	// Adopted synchronously...
	REQUIRE(client.isAuthenticated());
	REQUIRE(client.getAccessToken() == "token-1");
	REQUIRE(client.installedAppId() == "IA-1");
	// ...persisted and announced on the worker.
	REQUIRE(waitUntil([&]() { return valueIn(state, "installedAppId").value_or("") == "IA-1"; }));
	REQUIRE(waitUntil([&]() { return completed.load() == 1; }));
	REQUIRE(completedOk);

	auto authenticated = client.authenticate();
	REQUIRE(authenticated.isSuccess());
	REQUIRE(*authenticated.value == true);
	REQUIRE(client.waitForAuthentication(std::chrono::milliseconds(0)).isSuccess());

	// A refreshed token is adopted as-is: an empty id keeps the current one,
	// and a refresh is not a new sign-in (no second authenticationCompleted).
	client.setAccessToken("token-2");
	REQUIRE(client.getAccessToken() == "token-2");
	REQUIRE(client.installedAppId() == "IA-1");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	REQUIRE(completed.load() == 1);
}

TEST_CASE("An external-tokens Client rehydrates the installed-app id and persists it on shutdown", "[external-tokens]") {
	auto state = std::make_shared<SharedStorage::State>();
	state->values["installedAppId"] = "IA-stored";
	{
		Client client(ExternalTokens{}, nullptr, std::make_unique<SharedStorage>(state));
		REQUIRE(client.installedAppId() == "IA-stored");
		REQUIRE(client.authenticate().isSuccess()); // a null server is allowed

		// Destroyed right after: the id still reaches storage.
		client.setAccessToken("token-1", "IA-new");
	}
	REQUIRE(valueIn(state, "installedAppId").value_or("") == "IA-new");
}

TEST_CASE("setAccessToken flushes parked device subscriptions without blocking", "[external-tokens]") {
	FakeApi api("/installedapps/IA-1/subscriptions", "/installedapps/IA-1/subscriptions");
	REQUIRE(api.started);

	// The subscription create is held until the test releases it, to prove
	// setAccessToken() returns while the flush is still in flight.
	std::promise<void> release;
	std::shared_future<void> released = release.get_future().share();
	struct Releaser {
		std::promise<void>& promise;
		~Releaser() { try { promise.set_value(); } catch (...) {} }
	} releaser{ release };
	api.respondWith([released](const WebhookRequest&) {
		released.wait();
		return jsonResponse(200, R"({"id":"sub-1","installedAppId":"IA-1","sourceType":"DEVICE",)"
			R"("device":{"deviceId":"11111111-2222-3333-4444-555555555555","componentId":"*",)"
			R"("capability":"*","attribute":"*"}})");
		});

	Client client(ExternalTokens{}, std::make_unique<StubHttpServer>(), std::make_unique<SharedStorage>());
	client.setBaseUrl(api.baseUrl);

	// Signed out: the device's automatic subscription is parked, no request.
	auto device = makeLamp();
	client.registerDevice(device);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	REQUIRE(api.postCount() == 0);

	auto call = std::async(std::launch::async, [&]() { client.setAccessToken("token-1", "IA-1"); });
	REQUIRE(call.wait_for(std::chrono::seconds(2)) == std::future_status::ready);

	// The flush runs on the worker, with the new token.
	REQUIRE(waitUntil([&]() { return api.postCount() == 1; }));
	auto post = api.lastPost();
	REQUIRE(bearerOf(post) == "Bearer token-1");
	REQUIRE(json_utils::parse(post.body)["device"]["deviceId"] == kDeviceId);

	release.set_value();
	// Once created, the subscription is not re-created by the next refresh.
	client.setAccessToken("token-2");
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	REQUIRE(api.postCount() == 1);
}

TEST_CASE("A rejected token fires accessTokenRejected once, off the calling thread", "[external-tokens]") {
	FakeApi api("/unused", std::string("/devices/") + kDeviceId + "/commands");
	REQUIRE(api.started);
	api.respondWith([](const WebhookRequest&) { return jsonResponse(401, R"({"error":"unauthorized"})"); });

	RejectionLog log;
	Client client(ExternalTokens{}, std::make_unique<StubHttpServer>(), std::make_unique<SharedStorage>());
	client.setBaseUrl(api.baseUrl);

	auto rejectedSub = client.accessTokenRejected.SubscribeScoped(
		[&](std::string token) { log.record(token); });

	client.setAccessToken("stale-token", "IA-1");

	// Every request returns its auth error at once - no waiting for a token.
	for (int i = 0; i < 3; ++i) {
		auto result = client.executeCommands(kDeviceId, switchOnCommand());
		REQUIRE(result.error == ErrorCode::AuthenticationFailed);
	}
	REQUIRE(api.postCount() == 3);

	// One report for that token, however many requests it failed.
	REQUIRE(waitUntil([&]() { return log.count() == 1; }));
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	{
		std::lock_guard<std::mutex> lock(log.mutex);
		REQUIRE(log.tokens.size() == 1);
		REQUIRE(log.tokens[0] == "stale-token");
		REQUIRE(log.thread != std::this_thread::get_id());
	}

	// The next token gets its own report.
	client.setAccessToken("other-token");
	REQUIRE(client.executeCommands(kDeviceId, switchOnCommand()).error == ErrorCode::AuthenticationFailed);
	REQUIRE(waitUntil([&]() { return log.count() == 2; }));
	std::lock_guard<std::mutex> lock(log.mutex);
	REQUIRE(log.tokens[1] == "other-token");
}

TEST_CASE("A handler can refresh by calling setAccessToken; the next call succeeds", "[external-tokens]") {
	FakeApi api("/unused", std::string("/devices/") + kDeviceId + "/commands");
	REQUIRE(api.started);
	api.respondWith([](const WebhookRequest& request) {
		return bearerOf(request) == "Bearer fresh-token"
			? jsonResponse(200, R"({"results":[]})")
			: jsonResponse(401, R"({"error":"unauthorized"})");
		});

	Client client(ExternalTokens{}, std::make_unique<StubHttpServer>(), std::make_unique<SharedStorage>());
	client.setBaseUrl(api.baseUrl);

	// Re-entering the Client from the handler is fine: it runs on the worker.
	auto rejectedSub = client.accessTokenRejected.SubscribeScoped(
		[&](std::string) { client.setAccessToken("fresh-token"); });

	client.setAccessToken("stale-token", "IA-1");
	REQUIRE(client.executeCommands(kDeviceId, switchOnCommand()).error == ErrorCode::AuthenticationFailed);

	REQUIRE(waitUntil([&]() { return client.getAccessToken() == "fresh-token"; }));
	REQUIRE(client.executeCommands(kDeviceId, switchOnCommand()).isSuccess());
}

TEST_CASE("A 401 overtaken by a newer token is retried once with it, unreported", "[external-tokens]") {
	FakeApi api("/unused", std::string("/devices/") + kDeviceId + "/commands");
	REQUIRE(api.started);

	RejectionLog log;
	Client client(ExternalTokens{}, std::make_unique<StubHttpServer>(), std::make_unique<SharedStorage>());
	client.setBaseUrl(api.baseUrl);

	// The refresh lands while the first request is in flight.
	api.respondWith([&client](const WebhookRequest& request) {
		if (bearerOf(request) == "Bearer new-token") {
			return jsonResponse(200, R"({"results":[]})");
		}
		client.setAccessToken("new-token");
		return jsonResponse(401, R"({"error":"unauthorized"})");
		});

	auto rejectedSub = client.accessTokenRejected.SubscribeScoped(
		[&](std::string token) { log.record(token); });

	client.setAccessToken("old-token", "IA-1");
	REQUIRE(client.executeCommands(kDeviceId, switchOnCommand()).isSuccess());
	REQUIRE(api.postCount() == 2);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	REQUIRE(log.count() == 0);
}

TEST_CASE("PAT and OAuth Clients never fire accessTokenRejected", "[external-tokens]") {
	FakeApi api("/unused", std::string("/devices/") + kDeviceId + "/commands");
	REQUIRE(api.started);
	api.respondWith([](const WebhookRequest&) { return jsonResponse(401, R"({"error":"unauthorized"})"); });

	RejectionLog log;
	Client client("pat-token", PollingMode::Manual);
	client.setBaseUrl(api.baseUrl);

	auto rejectedSub = client.accessTokenRejected.SubscribeScoped(
		[&](std::string token) { log.record(token); });

	REQUIRE(client.executeCommands(kDeviceId, switchOnCommand()).error == ErrorCode::AuthenticationFailed);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	REQUIRE(log.count() == 0);
}

TEST_CASE("An empty token signs the Client out without deleting anything", "[external-tokens]") {
	// Every API route answers 404 here: the subscription the sign-in tries to
	// create simply stays parked, and nothing leaves the machine.
	FakeApi api("/unused", "/unused");
	REQUIRE(api.started);

	std::atomic<int> completed{ 0 };
	auto state = std::make_shared<SharedStorage::State>();
	auto server = std::make_unique<StubHttpServer>();
	auto* serverRaw = server.get();
	Client client(ExternalTokens{}, std::move(server), std::make_unique<SharedStorage>(state));
	client.setBaseUrl(api.baseUrl);
	client.setWebhookSignatureVerifier(nullptr); // unsigned test requests

	auto completedSub = client.authenticationCompleted.SubscribeScoped(
		[&](Result<void>) { ++completed; });

	auto device = makeLamp();
	client.registerDevice(device);

	client.setAccessToken("token-1", "IA-1");
	REQUIRE(waitUntil([&]() { return completed.load() == 1; }));
	REQUIRE(valueIn(state, "installedAppId").value_or("") == "IA-1");

	client.setAccessToken("");

	REQUIRE_FALSE(client.isAuthenticated());
	REQUIRE(client.getAccessToken().empty());
	REQUIRE(client.executeCommands(kDeviceId, switchOnCommand()).error == ErrorCode::AuthenticationRequired);
	REQUIRE(client.validateAuthentication().error == ErrorCode::AuthenticationRequired);
	REQUIRE(client.getDevices().empty());
	REQUIRE(*client.authenticate().value == false);
	REQUIRE(client.waitForAuthentication(std::chrono::seconds(1)).error == ErrorCode::AuthenticationRequired);
	SubscriptionRequest request;
	request.deviceId = kDeviceId;
	REQUIRE(client.subscribeTo(request).error == ErrorCode::AuthenticationRequired);

	// Nothing was deleted: the installed-app id stays (in memory and stored)
	// and registered Devices still receive pushed events.
	REQUIRE(client.installedAppId() == "IA-1");
	REQUIRE(valueIn(state, "installedAppId").value_or("") == "IA-1");
	WebhookRequest event;
	event.path = "/webhook";
	event.body = kSwitchOnEvent;
	REQUIRE(serverRaw->simulateWebhook(event).statusCode == 200);
	auto* sw = device->getCapability<standard::Switch>();
	REQUIRE(sw != nullptr);
	REQUIRE(sw->statusJson()["switch"]["value"] == "on");

	// Signing in again is announced again.
	client.setAccessToken("token-2");
	REQUIRE(waitUntil([&]() { return completed.load() == 2; }));
	REQUIRE(client.isAuthenticated());
}

TEST_CASE("Webhooks reach an external-tokens Client through the injected IHttpServer", "[external-tokens][webhook]") {
	std::atomic<int> changes{ 0 };
	auto server = std::make_unique<StubHttpServer>();
	auto* serverRaw = server.get();
	Client client(ExternalTokens{}, std::move(server), std::make_unique<SharedStorage>());

	auto device = makeLamp();
	client.registerDevice(device);
	auto changedSub = device->PropertyChanged.SubscribeScoped(
		[&](ObservableObject&, PropertyChangedArgs) { ++changes; });

	WebhookRequest event;
	event.path = "/webhook";
	event.body = kSwitchOnEvent;

	SECTION("HTTP-Signature verification is installed by default") {
		auto response = serverRaw->simulateWebhook(event); // unsigned
		REQUIRE(response.statusCode == 401);
		REQUIRE(changes.load() == 0);
	}

	SECTION("PING, INSTALL and EVENT are handled once verification is out of the way") {
		// Tests cannot produce SmartThings signatures (see webhook_signature_tests).
		client.setWebhookSignatureVerifier(nullptr);

		WebhookRequest ping;
		ping.path = "/webhook";
		ping.body = R"({"messageType":"PING","pingData":{"challenge":"CH-1"}})";
		auto pong = serverRaw->simulateWebhook(ping);
		REQUIRE(pong.messageType == WebhookMessageType::Ping);
		REQUIRE(json_utils::parse(pong.body)["pingData"]["challenge"] == "CH-1");

		WebhookRequest install;
		install.path = "/webhook";
		install.body = R"({"messageType":"INSTALL","installData":{"installedApp":{"installedAppId":"IA-9"}}})";
		REQUIRE(serverRaw->simulateWebhook(install).messageType == WebhookMessageType::Install);
		REQUIRE(client.installedAppId() == "IA-9");

		auto response = serverRaw->simulateWebhook(event);
		REQUIRE(response.statusCode == 200);
		REQUIRE(response.messageType == WebhookMessageType::Event);
		REQUIRE(changes.load() > 0);
		auto* sw = device->getCapability<standard::Switch>();
		REQUIRE(sw != nullptr);
		REQUIRE(sw->statusJson()["switch"]["value"] == "on");
	}
}
