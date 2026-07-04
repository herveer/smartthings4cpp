#include "smartthings4cpp/webhook.h"
#include "smartthings4cpp/client.h"
#include "smartthings4cpp/device.h"
#include "smartthings4cpp/http_client.h"
#include "smartthings4cpp/json_utils.h"

#include <nlohmann/json.hpp>
#include <cctype>
#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <utility>

/**
 * @file webhook.cpp
 * @brief Client's OAuth event surface: subscription CRUD + the webhook
 *        messageType handler (see webhook.h / client.h). Kept out of client.cpp
 *        so the messageType state machine doesn't crowd auth/discovery/polling.
 */

namespace smartthings4cpp {

	namespace {
		// Matches client.cpp's own timeout for cloud calls.
		constexpr std::chrono::milliseconds REQUEST_TIMEOUT{ 10000 };

		// Keep only characters SmartThings allows in a subscriptionName, capped to
		// maxLen. Keeps the tail of long inputs (a device id's tail is the most
		// distinguishing part) so per-device names stay unique.
		std::string sanitizeName(const std::string& in, size_t maxLen) {
			std::string out;
			for (char c : in) {
				if (std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_') {
					out += c;
				}
			}
			if (out.size() > maxLen) {
				out = out.substr(out.size() - maxLen);
			}
			return out;
		}

		std::string makeSubscriptionName(const SubscriptionRequest& request) {
			std::string dev = sanitizeName(request.deviceId, 30);
			std::string name = "st_all_" + dev;
			if (name.size() > 36) {
				name = name.substr(0, 36);
			}
			return name;
		}

		Subscription parseSubscription(const nlohmann::json& j) {
			Subscription s;
			if (!j.is_object()) {
				return s;
			}
			s.id = json_utils::getValueOr<std::string>(j, "id", "");
			s.installedAppId = json_utils::getValueOr<std::string>(j, "installedAppId", "");
			s.sourceType = json_utils::getValueOr<std::string>(j, "sourceType", "");
			if (j.contains("device") && j["device"].is_object()) {
				const auto& d = j["device"];
				s.deviceId = json_utils::getValueOr<std::string>(d, "deviceId", "");
				s.capability = json_utils::getValueOr<std::string>(d, "capability", "");
				s.attribute = json_utils::getValueOr<std::string>(d, "attribute", "");
				s.componentId = json_utils::getValueOr<std::string>(d, "componentId", "");
			}
			return s;
		}

		// Map a non-2xx subscription response to a Result error. Works for any
		// Result<T> (including void) since they share the (ErrorCode, msg) ctor.
		template<typename T>
		Result<T> subscriptionError(const HttpResponse& r, const std::string& action) {
			switch (r.status_code) {
			case 401:
			case 403:
				return Result<T>(ErrorCode::AuthenticationFailed,
					action + ": token invalid or lacks the subscription scope");
			case 404:
				return Result<T>(ErrorCode::ResourceNotFound,
					action + ": installed app or subscription not found");
			case 429:
				return Result<T>(ErrorCode::RateLimited, action + ": rate limit exceeded");
			case 0:
				return Result<T>(ErrorCode::NetworkError,
					action + ": failed to reach the SmartThings API: " + r.error_message);
			default:
				return Result<T>(ErrorCode::ApiError,
					action + ": unexpected status " + std::to_string(r.status_code));
			}
		}
	} // namespace

	nlohmann::json buildSubscriptionBody(const SubscriptionRequest& request) {
		std::string name = request.subscriptionName.empty()
			? makeSubscriptionName(request) : request.subscriptionName;

		nlohmann::json device = {
			{ "deviceId", request.deviceId },
			{ "stateChangeOnly", request.stateChangeOnly }
		};

		return nlohmann::json{ { "sourceType", "DEVICE" }, { "device", std::move(device) }, { "subscriptionName", name } };
	}

	// --- installed-app identity / token ------------------------------------

	void Client::setInstalledAppId(const std::string& installedAppId) {
		std::lock_guard<std::mutex> lock(_installMutex);
		_installedAppId = installedAppId;
	}

	std::string Client::installedAppId() const {
		std::lock_guard<std::mutex> lock(_installMutex);
		return _installedAppId;
	}

	std::string Client::subscriptionToken() const {
		{
			std::lock_guard<std::mutex> lock(_installMutex);
			if (!_installedAppToken.empty()) {
				return _installedAppToken;
			}
		}
		std::lock_guard<std::mutex> lock(_authMutex);
		return _access_token;
	}

	// --- subscription CRUD -------------------------------------------------

	Result<Subscription> Client::subscribeTo(const SubscriptionRequest& request) {
		if (!_isOAuth) {
			return Result<Subscription>(ErrorCode::InvalidRequest,
				"subscriptions require an OAuth-mode Client");
		}
		const std::string appId = installedAppId();
		if (appId.empty()) {
			return Result<Subscription>(ErrorCode::InvalidParameter,
				"no installedAppId yet - process the Install webhook or call setInstalledAppId()");
		}
		if (request.deviceId.empty()) {
			return Result<Subscription>(ErrorCode::InvalidParameter,
				"SubscriptionRequest.deviceId is empty");
		}
		const std::string token = subscriptionToken();
		if (token.empty()) {
			return Result<Subscription>(ErrorCode::AuthenticationRequired, "no access token set");
		}

		try {
			auto headers = authHeadersFor(token);
			headers["Content-Type"] = "application/json";

			HttpClient http;
			http.setTimeout(REQUEST_TIMEOUT);
			auto response = http.post(
				_base_url + "/installedapps/" + appId + "/subscriptions",
				buildSubscriptionBody(request).dump(), headers);

			if (response.isSuccess()) {
				return Result<Subscription>(parseSubscription(json_utils::parse(response.body)));
			}
			return subscriptionError<Subscription>(response, "subscribeTo");
		}
		catch (const std::exception& e) {
			return Result<Subscription>(ErrorCode::UnknownError,
				std::string("subscribeTo error: ") + e.what());
		}
	}

	Result<int> Client::subscribeToAllDevices() {
		if (!_isOAuth) {
			return Result<int>(ErrorCode::InvalidRequest,
				"subscriptions require an OAuth-mode Client");
		}
		if (installedAppId().empty()) {
			return Result<int>(ErrorCode::InvalidParameter,
				"no installedAppId yet - process the Install webhook or call setInstalledAppId()");
		}

		int count = 0;
		Result<Subscription> last;
		auto snapshot = snapshotRegistry();
		for (auto& entry : snapshot) {
			SubscriptionRequest request;
			request.deviceId = entry.first; // capability/attribute default to "*"
			last = subscribeTo(request);
			if (last.isSuccess()) {
				++count;
			}
		}

		// Surface a systemic failure (bad token/appId) rather than silently
		// returning 0; a partial success still reports how many were created.
		if (count == 0 && !snapshot.empty() && !last.isSuccess()) {
			return Result<int>(last.error, last.error_message);
		}
		return Result<int>(count);
	}

	Result<std::vector<Subscription>> Client::listSubscriptions() {
		using ListResult = Result<std::vector<Subscription>>;
		if (!_isOAuth) {
			return ListResult(ErrorCode::InvalidRequest,
				"subscriptions require an OAuth-mode Client");
		}
		const std::string appId = installedAppId();
		if (appId.empty()) {
			return ListResult(ErrorCode::InvalidParameter,
				"no installedAppId yet - process the Install webhook or call setInstalledAppId()");
		}
		const std::string token = subscriptionToken();
		if (token.empty()) {
			return ListResult(ErrorCode::AuthenticationRequired, "no access token set");
		}

		try {
			HttpClient http;
			http.setTimeout(REQUEST_TIMEOUT);
			auto response = http.get(
				_base_url + "/installedapps/" + appId + "/subscriptions", authHeadersFor(token));

			if (!response.isSuccess()) {
				return subscriptionError<std::vector<Subscription>>(response, "listSubscriptions");
			}

			std::vector<Subscription> subs;
			auto json = json_utils::parse(response.body);
			if (json.is_object() && json.contains("items") && json["items"].is_array()) {
				for (const auto& item : json["items"]) {
					subs.push_back(parseSubscription(item));
				}
			}
			return ListResult(std::move(subs));
		}
		catch (const std::exception& e) {
			return ListResult(ErrorCode::UnknownError,
				std::string("listSubscriptions error: ") + e.what());
		}
	}

	Result<void> Client::deleteSubscription(const std::string& subscriptionId) {
		if (!_isOAuth) {
			return Result<void>(ErrorCode::InvalidRequest,
				"subscriptions require an OAuth-mode Client");
		}
		const std::string appId = installedAppId();
		if (appId.empty()) {
			return Result<void>(ErrorCode::InvalidParameter,
				"no installedAppId yet - process the Install webhook or call setInstalledAppId()");
		}
		if (subscriptionId.empty()) {
			return Result<void>(ErrorCode::InvalidParameter, "subscriptionId is empty");
		}
		const std::string token = subscriptionToken();
		if (token.empty()) {
			return Result<void>(ErrorCode::AuthenticationRequired, "no access token set");
		}

		try {
			HttpClient http;
			http.setTimeout(REQUEST_TIMEOUT);
			auto response = http.del(
				_base_url + "/installedapps/" + appId + "/subscriptions/" + subscriptionId,
				authHeadersFor(token));

			if (response.isSuccess()) {
				return Result<void>();
			}
			return subscriptionError<void>(response, "deleteSubscription");
		}
		catch (const std::exception& e) {
			return Result<void>(ErrorCode::UnknownError,
				std::string("deleteSubscription error: ") + e.what());
		}
	}

	Result<void> Client::deleteAllSubscriptions() {
		if (!_isOAuth) {
			return Result<void>(ErrorCode::InvalidRequest,
				"subscriptions require an OAuth-mode Client");
		}
		const std::string appId = installedAppId();
		if (appId.empty()) {
			return Result<void>(ErrorCode::InvalidParameter,
				"no installedAppId yet - process the Install webhook or call setInstalledAppId()");
		}
		const std::string token = subscriptionToken();
		if (token.empty()) {
			return Result<void>(ErrorCode::AuthenticationRequired, "no access token set");
		}

		try {
			HttpClient http;
			http.setTimeout(REQUEST_TIMEOUT);
			auto response = http.del(
				_base_url + "/installedapps/" + appId + "/subscriptions", authHeadersFor(token));

			if (response.isSuccess()) {
				return Result<void>();
			}
			return subscriptionError<void>(response, "deleteAllSubscriptions");
		}
		catch (const std::exception& e) {
			return Result<void>(ErrorCode::UnknownError,
				std::string("deleteAllSubscriptions error: ") + e.what());
		}
	}

	// --- event dispatch ----------------------------------------------------

	std::vector<std::shared_ptr<Device>> Client::liveDevicesFor(const std::string& deviceId) {
		std::vector<std::shared_ptr<Device>> live;

		std::lock_guard<std::mutex> lock(_registryMutex);
		auto it = _deviceRegistry.find(deviceId);
		if (it == _deviceRegistry.end()) {
			return live;
		}

		auto& bucket = it->second;
		for (auto& w : bucket) {
			if (auto locked = w.lock()) {
				live.push_back(std::move(locked));
			}
		}

		if (live.empty()) {
			_deviceRegistry.erase(it); // nothing left for this id: drop the bucket
			return live;
		}

		// Compact the bucket to just the survivors while we hold the lock.
		bucket.clear();
		for (auto& d : live) {
			bucket.push_back(d);
		}
		return live;
	}

	// --- webhook messageType -------------------------------------------------

	WebhookResponse Client::handleWebhook(const std::string& rawBody) {
		WebhookResponse resp;

		nlohmann::json body;
		try {
			body = json_utils::parse(rawBody);
		}
		catch (const std::exception&) {
			resp.statusCode = 400;
			resp.body = R"({"error":"request body is not valid JSON"})";
			return resp;
		}

		if (!body.is_object() || !body.contains("messageType")) {
			resp.statusCode = 400;
			resp.body = R"({"error":"missing messageType"})";
			return resp;
		}

		const std::string messageType = json_utils::getValueOr<std::string>(body, "messageType", "");

		if (messageType == "PING") {
			resp.messageType = WebhookMessageType::Ping;
			std::string challenge;
			if (body.contains("pingData") && body["pingData"].is_object()) {
				challenge = json_utils::getValueOr<std::string>(body["pingData"], "challenge", "");
			}
			resp.body = nlohmann::json{ { "pingData", { { "challenge", challenge } } } }.dump();
			return resp;
		}

		if (messageType == "CONFIRMATION") {
			resp.messageType = WebhookMessageType::Confirmation;
			std::string confirmationUrl;
			if (body.contains("confirmationData") && body["confirmationData"].is_object()) {
				confirmationUrl = json_utils::getValueOr<std::string>(
					body["confirmationData"], "confirmationUrl", "");
			}
			// Visiting the confirmationUrl is what actually confirms Target-URL
			// ownership; the response to this POST just needs to be 200.
			if (!confirmationUrl.empty()) {
				try {
					HttpClient http;
					http.setTimeout(REQUEST_TIMEOUT);
					http.get(confirmationUrl);
				}
				catch (const std::exception&) {
					// Best effort; SmartThings retries CONFIRMATION on failure.
				}
			}
			resp.body = "{}";
			return resp;
		}

		if (messageType == "CONFIGURATION") {
			resp.messageType = WebhookMessageType::Configuration;
			std::string phase;
			if (body.contains("configurationData") && body["configurationData"].is_object()) {
				phase = json_utils::getValueOr<std::string>(body["configurationData"], "phase", "");
			}
			// Minimal single empty page, valid for both phases, so an install that
			// does route through CONFIGURATION can still proceed.
			if (phase == "PAGE") {
				resp.body = R"({"configurationData":{"page":{"pageId":"1","name":"smartthings4cpp",)"
					R"("nextPageId":null,"previousPageId":null,"complete":true,"sections":[]}}})";
			}
			else {
				resp.body = R"({"configurationData":{"initialize":{"name":"smartthings4cpp",)"
					R"("description":"smartthings4cpp events","id":"app","permissions":[],"firstPageId":"1"}}})";
			}
			return resp;
		}

		if (messageType == "INSTALL" || messageType == "UPDATE") {
			const bool isInstall = (messageType == "INSTALL");
			resp.messageType = isInstall ? WebhookMessageType::Install : WebhookMessageType::Update;
			const char* dataKey = isInstall ? "installData" : "updateData";

			std::string appId;
			std::string token;
			if (body.contains(dataKey) && body[dataKey].is_object()) {
				const auto& data = body[dataKey];
				token = json_utils::getValueOr<std::string>(data, "authToken", "");
				if (data.contains("installedApp") && data["installedApp"].is_object()) {
					appId = json_utils::getValueOr<std::string>(
						data["installedApp"], "installedAppId", "");
				}
			}

			{
				std::lock_guard<std::mutex> lock(_installMutex);
				if (!appId.empty()) {
					_installedAppId = appId;
				}
				if (!token.empty()) {
					_installedAppToken = token;
				}
			}
			if (!appId.empty() && _storage) {
				_storage->setValue("installedAppId", appId);
			}
			// The id/token may have just become available: create any automatic
			// subscriptions that were parked waiting for them (outside
			// _installMutex, since subscription calls read it themselves).
			flushPendingSubscriptions();

			resp.body = isInstall ? R"({"installData":{}})" : R"({"updateData":{}})";
			return resp;
		}

		if (messageType == "EVENT") {
			resp.messageType = WebhookMessageType::Event;
			if (body.contains("eventData") && body["eventData"].is_object()) {
				const auto& eventData = body["eventData"];
				if (eventData.contains("events") && eventData["events"].is_array()) {
					for (const auto& ev : eventData["events"]) {
						if (!ev.is_object()) {
							continue;
						}
						const std::string type = json_utils::getValueOr<std::string>(ev, "eventType", "");
						if (type != "DEVICE_EVENT" || !ev.contains("deviceEvent")
							|| !ev["deviceEvent"].is_object()) {
							continue;
						}
						const auto& de = ev["deviceEvent"];
						const std::string deviceId = json_utils::getValueOr<std::string>(de, "deviceId", "");
						const std::string componentId = json_utils::getValueOr<std::string>(de, "componentId", "main");
						const std::string capability = json_utils::getValueOr<std::string>(de, "capability", "");
						const std::string attribute = json_utils::getValueOr<std::string>(de, "attribute", "");
						if (deviceId.empty() || capability.empty() || attribute.empty()) {
							continue;
						}
						nlohmann::json value = de.contains("value")
							? de["value"] : nlohmann::json(nullptr);

						// One event, fanned out to every live Device for that id.
						for (auto& device : liveDevicesFor(deviceId)) {
							device->applyDeviceEvent(componentId, capability, attribute, value);
						}
					}
				}
			}
			resp.body = R"({"eventData":{}})";
			return resp;
		}

		if (messageType == "UNINSTALL") {
			resp.messageType = WebhookMessageType::Uninstall;
			{
				std::lock_guard<std::mutex> lock(_installMutex);
				_installedAppId.clear();
				_installedAppToken.clear();
			}
			// The installation - and every token/subscription attached to it -
			// is gone: forget the persisted state too, so the next authenticate()
			// starts a fresh consent instead of replaying dead tokens.
			if (_isOAuth) {
				{
					std::lock_guard<std::mutex> lock(_authMutex);
					_access_token.clear();
				}
				if (_keychain) {
					_keychain->removeValue("oauth.token");
				}
				if (_storage) {
					_storage->removeValue("installedAppId");
				}
				{
					std::lock_guard<std::mutex> lock(_subscriptionMutex);
					_subscribedDevices.clear();
					_pendingSubscriptions.clear();
					_unclaimedSubscriptions.clear();
					_unclaimedLoaded = false;
				}
			}
			resp.body = R"({"uninstallData":{}})";
			return resp;
		}

		if (messageType == "OAUTH_CALLBACK") {
			resp.messageType = WebhookMessageType::OAuthCallback;
			resp.body = R"({"oAuthCallbackData":{}})";
			return resp;
		}

		// Unknown messageType: acknowledge without erroring so a newly-added phase
		// doesn't take the endpoint down.
		resp.body = "{}";
		return resp;
	}

	// --- automatic subscription messageType ------------------------------------
	// Consumers never call these: registerDevice() subscribes each new device
	// id (adopting a prior run's subscription when possible), Install/auth
	// completion flushes the ones that had to wait, and onDeviceReleased()
	// (client.cpp) deletes a subscription when its last Device dies.

	void Client::maybeSubscribeDevice(const std::string& deviceId) {
		if (!_isOAuth || deviceId.empty()) {
			return;
		}
		{
			std::lock_guard<std::mutex> lock(_subscriptionMutex);
			if (_subscribedDevices.count(deviceId) != 0
				|| _subscriptionsInFlight.count(deviceId) != 0) {
				return;
			}
			if (!isAuthenticated() || installedAppId().empty()) {
				// Not ready yet: parked, flushed once authentication/Install
				// delivers the missing pieces.
				_pendingSubscriptions.insert(deviceId);
				return;
			}
			_subscriptionsInFlight.insert(deviceId);
		}

		loadUnclaimedSubscriptionsOnce();

		// Adopt a prior run's subscription for this device when one exists -
		// zero writes against the rate-limited (40/15min) subscription API.
		std::string adopted;
		{
			std::lock_guard<std::mutex> lock(_subscriptionMutex);
			auto it = _unclaimedSubscriptions.find(deviceId);
			if (it != _unclaimedSubscriptions.end()) {
				adopted = it->second;
				_unclaimedSubscriptions.erase(it);
			}
		}
		if (!adopted.empty()) {
			std::lock_guard<std::mutex> lock(_subscriptionMutex);
			_subscribedDevices[deviceId] = adopted;
			_subscriptionsInFlight.erase(deviceId);
			_pendingSubscriptions.erase(deviceId);
			return;
		}

		SubscriptionRequest request;
		request.deviceId = deviceId; // capability/attribute default to "*"
		auto created = subscribeTo(request);

		std::lock_guard<std::mutex> lock(_subscriptionMutex);
		_subscriptionsInFlight.erase(deviceId);
		if (created.isSuccess() && created.hasValue()) {
			_subscribedDevices[deviceId] = created.value->id;
			_pendingSubscriptions.erase(deviceId);
		}
		else {
			// Includes 429 rate-limiting: parked, retried on the next
			// registration or authentication event.
			_pendingSubscriptions.insert(deviceId);
		}
	}

	void Client::flushPendingSubscriptions() {
		if (!_isOAuth) {
			return;
		}
		std::vector<std::string> pending;
		{
			std::lock_guard<std::mutex> lock(_subscriptionMutex);
			pending.assign(_pendingSubscriptions.begin(), _pendingSubscriptions.end());
			_pendingSubscriptions.clear();
		}
		for (const auto& deviceId : pending) {
			maybeSubscribeDevice(deviceId); // re-parks itself on failure
		}
	}

	void Client::loadUnclaimedSubscriptionsOnce() {
		{
			std::lock_guard<std::mutex> lock(_subscriptionMutex);
			if (_unclaimedLoaded) {
				return;
			}
			_unclaimedLoaded = true; // claimed before the network call: one attempt at a time
		}
		auto listed = listSubscriptions();
		std::lock_guard<std::mutex> lock(_subscriptionMutex);
		if (!listed.isSuccess() || !listed.hasValue()) {
			_unclaimedLoaded = false; // retry on the next opportunity
			return;
		}
		for (const auto& subscription : *listed.value) {
			if (!subscription.deviceId.empty() && !subscription.id.empty()
				&& _subscribedDevices.count(subscription.deviceId) == 0) {
				if (subscription.componentId == "*" && subscription.capability == "*" && subscription.attribute == "*") {
					_unclaimedSubscriptions[subscription.deviceId] = subscription.id;
				}
				else {
					deleteSubscription(subscription.id);
				}
			}
		}
	}

} // namespace smartthings4cpp
