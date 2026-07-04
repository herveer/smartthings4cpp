#include "smartthings4cpp/client.h"
#include "smartthings4cpp/device.h"
#include "smartthings4cpp/http_client.h"
#include "smartthings4cpp/json_utils.h"
#include "smartthings4cpp/oauth2/oauth2_authenticator.h"

#include <nlohmann/json.hpp>
#include <algorithm>
#include <chrono>
#include <utility>

namespace smartthings4cpp {

	namespace {
		// Default timeout for cloud API calls.
		constexpr std::chrono::milliseconds REQUEST_TIMEOUT{ 10000 };

		// Extract the scheme://host origin from a base URL so we can resolve
		// relative pagination links. "https://api.smartthings.com/v1" -> "https://api.smartthings.com".
		std::string originOf(const std::string& base_url) {
			auto scheme_pos = base_url.find("://");
			if (scheme_pos == std::string::npos) {
				return base_url;
			}
			auto host_start = scheme_pos + 3;
			auto path_pos = base_url.find('/', host_start);
			if (path_pos == std::string::npos) {
				return base_url;
			}
			return base_url.substr(0, path_pos);
		}

		// Resolve a pagination "href" (which may be absolute or relative) against
		// the configured base URL.
		std::string resolveNextUrl(const std::string& base_url, const std::string& href) {
			if (href.empty()) {
				return "";
			}
			if (href.rfind("http://", 0) == 0 || href.rfind("https://", 0) == 0) {
				return href;
			}
			if (href.front() == '/') {
				return originOf(base_url) + href;
			}
			return base_url + "/" + href;
		}
	}

	/**
	 * Bridges shared_ptr<Device> custom deleters back to their Client. Each
	 * deleter only holds a weak_ptr to this hub: once ~Client() has disarmed it
	 * (client = nullptr) - or the hub itself is gone - a Device destroyed after
	 * its Client is a silent no-op instead of a call into freed memory.
	 */
	struct Client::DeviceLifetimeHub {
		std::mutex mutex;
		Client* client = nullptr; // guarded by mutex

		void notifyReleased(const std::string& deviceId) {
			std::lock_guard<std::mutex> lock(mutex);
			if (client) {
				client->onDeviceReleased(deviceId);
			}
		}
	};

	Client::Client(PollingMode pollingMode) {
		_lifetimeHub = std::make_shared<DeviceLifetimeHub>();
		_lifetimeHub->client = this;
		if (pollingMode == PollingMode::Automatic) {
			startPolling();
		}
	}

	Client::Client(const std::string& access_token, PollingMode pollingMode)
		: _access_token(access_token) {
		_lifetimeHub = std::make_shared<DeviceLifetimeHub>();
		_lifetimeHub->client = this;
		if (pollingMode == PollingMode::Automatic) {
			startPolling();
		}
	}

	Client::Client(oauth2::OAuth2Config config, int port, std::string oauthCallbackRoute,
		std::string webhookCallbackRoute, std::string externalUri, std::filesystem::path workingDir)
		: Client(std::move(config),
			makeDefaultHttpServer(port, std::move(oauthCallbackRoute), std::move(webhookCallbackRoute), std::move(externalUri)),
			makeDefaultKeychainProvider(workingDir),
			makeDefaultStorageProvider(workingDir)) {
	}

	Client::Client(oauth2::OAuth2Config config, std::unique_ptr<IHttpServer> httpServer,
		std::unique_ptr<IStorageProvider> keychainProvider,
		std::unique_ptr<IStorageProvider> storageProvider)
		: _isOAuth(true),
		_oauthConfig(std::move(config)),
		_authenticator(std::make_unique<oauth2::OAuth2Authenticator>(_oauthConfig)),
		_httpServer(std::move(httpServer)),
		_keychain(std::move(keychainProvider)),
		_storage(std::move(storageProvider)) {
		_lifetimeHub = std::make_shared<DeviceLifetimeHub>();
		_lifetimeHub->client = this;

		// Rehydrate persisted state so a second run authenticates silently.
		if (_storage) {
			if (auto appId = _storage->getValue("installedAppId")) {
				std::lock_guard<std::mutex> lock(_installMutex);
				_installedAppId = *appId;
			}
		}
		if (auto stored = loadPersistedOAuthToken()) {
			if (!stored->accessToken.empty()) {
				_access_token = stored->accessToken; // no other threads yet
			}
			if (!stored->installedAppId.empty()) {
				std::lock_guard<std::mutex> lock(_installMutex);
				_installedAppId = stored->installedAppId;
			}
		}

		// Transparent 401 recovery through the existing getWithAuth/postWithAuth
		// retry chokepoints. Deliberately minimal - refresh, persist, return -
		// because it runs while getWithAuth() already holds _authMutex (which
		// also assigns the returned token itself).
		_token_refresh_callback = [this]() -> Result<std::string> {
			auto stored = loadPersistedOAuthToken();
			if (!stored || stored->refreshToken.empty()) {
				return Result<std::string>(ErrorCode::AuthenticationRequired,
					"No stored refresh token to recover with");
			}
			auto refreshed = _authenticator->refresh(stored->refreshToken);
			if (!refreshed.isSuccess() || !refreshed.hasValue()) {
				return Result<std::string>(refreshed.error, refreshed.error_message);
			}
			persistOAuthToken(*refreshed.value);
			return Result<std::string>(refreshed.value->accessToken);
			};

		if (_httpServer) {
			_httpServer->bind(
				[this](std::string args) { onOAuthRedirect(std::move(args)); },
				[this](nlohmann::json body) -> nlohmann::json {
					auto response = handleWebhook(body.dump());
					try {
						return json_utils::parse(response.body);
					}
					catch (const std::exception&) {
						return nlohmann::json::object();
					}
				});
			_serverStart = _httpServer->start();
		}
		else {
			_serverStart = Result<void>(ErrorCode::InvalidParameter,
				"No IHttpServer provided to the OAuth Client");
		}
		// No background polling in OAuth mode: webhook events drive updates.
	}

	Client::~Client() {
		// Stop inbound callbacks first: the server thread calls back into this
		// Client (handleWebhook/onOAuthRedirect), so it must be quiet before
		// anything else is torn down.
		if (_httpServer) {
			_httpServer->stop();
		}
		stopPolling();

		// Disarm the deleter hub: a Device destroyed after this point no-ops.
		if (_lifetimeHub) {
			std::lock_guard<std::mutex> lock(_lifetimeHub->mutex);
			_lifetimeHub->client = nullptr;
		}

		// Best effort: tear down automatic subscriptions still registered. In a
		// clean shutdown the Devices died first and already removed theirs, so
		// this is normally empty (crash leftovers are adopted by the next run).
		if (_isOAuth) {
			std::unordered_map<std::string, std::string> remaining;
			{
				std::lock_guard<std::mutex> lock(_subscriptionMutex);
				remaining.swap(_subscribedDevices);
			}
			for (auto& [deviceId, subscriptionId] : remaining) {
				(void)deviceId;
				if (!subscriptionId.empty()) {
					deleteSubscription(subscriptionId);
				}
			}
		}
	}

	void Client::setAccessToken(const std::string& token) {
		_access_token = token;
	}

	std::string Client::getAccessToken() const {
		return _access_token;
	}

	bool Client::isAuthenticated() const {
		return !_access_token.empty();
	}

	void Client::setTokenRefreshCallback(std::function<Result<std::string>()> callback) {
		_token_refresh_callback = std::move(callback);
	}

	void Client::setBaseUrl(const std::string& url) {
		_base_url = url;
	}

	const std::string& Client::getBaseUrl() const {
		return _base_url;
	}

	std::map<std::string, std::string> Client::authHeadersFor(const std::string& token) const {
		return {
			{ "Authorization", "Bearer " + token },
			{ "Accept", "application/json" }
		};
	}

	HttpResponse Client::getWithAuth(const std::string& url) const {
		HttpClient client;
		client.setTimeout(REQUEST_TIMEOUT);

		std::string tokenSnapshot;
		{
			std::lock_guard<std::mutex> lock(_authMutex);
			tokenSnapshot = _access_token;
		}

		auto response = client.get(url, authHeadersFor(tokenSnapshot));
		if (response.status_code == 401 && _token_refresh_callback) {
			std::lock_guard<std::mutex> lock(_authMutex);
			if (_access_token != tokenSnapshot) {
				// Another thread already refreshed while our request was in
				// flight; just retry with the token that's current now instead
				// of refreshing a second time for the same expiry event.
				response = client.get(url, authHeadersFor(_access_token));
			}
			else {
				auto refreshed = _token_refresh_callback();
				if (refreshed.isSuccess() && refreshed.hasValue()) {
					_access_token = *refreshed.value;
					response = client.get(url, authHeadersFor(_access_token));
				}
			}
		}
		return response;
	}

	HttpResponse Client::postWithAuth(const std::string& url, const std::string& body) const {
		HttpClient client;
		client.setTimeout(REQUEST_TIMEOUT);

		std::string tokenSnapshot;
		{
			std::lock_guard<std::mutex> lock(_authMutex);
			tokenSnapshot = _access_token;
		}

		auto response = client.post(url, body, authHeadersFor(tokenSnapshot));
		if (response.status_code == 401 && _token_refresh_callback) {
			std::lock_guard<std::mutex> lock(_authMutex);
			if (_access_token != tokenSnapshot) {
				response = client.post(url, body, authHeadersFor(_access_token));
			}
			else {
				auto refreshed = _token_refresh_callback();
				if (refreshed.isSuccess() && refreshed.hasValue()) {
					_access_token = *refreshed.value;
					response = client.post(url, body, authHeadersFor(_access_token));
				}
			}
		}
		return response;
	}

	Result<void> Client::validateAuthentication() const {
		if (_access_token.empty()) {
			return Result<void>(ErrorCode::AuthenticationRequired,
				"No access token set");
		}

		try {
			auto response = getWithAuth(_base_url + "/devices?max=1");

			if (response.isSuccess()) {
				return Result<void>();
			}

			if (response.status_code == 401 || response.status_code == 403) {
				return Result<void>(ErrorCode::AuthenticationFailed,
					"Access token is invalid or lacks the required scope");
			}

			if (response.status_code == 429) {
				return Result<void>(ErrorCode::RateLimited,
					"Rate limit exceeded while validating token");
			}

			if (response.status_code == 0) {
				return Result<void>(ErrorCode::NetworkError,
					"Failed to reach the SmartThings API: " + response.error_message);
			}

			return Result<void>(ErrorCode::ApiError,
				"Unexpected status " + std::to_string(response.status_code)
				+ " while validating token");
		}
		catch (const std::exception& e) {
			return Result<void>(ErrorCode::UnknownError,
				std::string("Validation error: ") + e.what());
		}
	}

	nlohmann::json Client::fetchAllItems(const std::string& path) const {
		nlohmann::json items = nlohmann::json::array();

		if (!isAuthenticated()) {
			return items;
		}

		try {
			std::string url = _base_url + path;

			// Follow _links.next until the API stops handing out pages. The bound
			// is a safety net against a misbehaving server returning a cycle.
			for (int page = 0; page < 1000 && !url.empty(); ++page) {
				auto response = getWithAuth(url);
				if (!response.isSuccess()) {
					break;
				}

				auto json_response = json_utils::parse(response.body);

				if (json_response.contains("items") && json_response["items"].is_array()) {
					for (const auto& item : json_response["items"]) {
						items.push_back(item);
					}
				}

				url.clear();
				if (json_response.contains("_links")
					&& json_response["_links"].is_object()
					&& json_response["_links"].contains("next")
					&& json_response["_links"]["next"].is_object()) {
					std::string href = json_utils::getValueOr<std::string>(
						json_response["_links"]["next"], "href", "");
					url = resolveNextUrl(_base_url, href);
				}
			}
		}
		catch (const std::exception&) {
			// Defensive: return whatever was accumulated so far (possibly empty).
		}

		return items;
	}

	std::vector<std::shared_ptr<Device>> Client::getDevices() {
		std::vector<std::shared_ptr<Device>> devices;

		auto items = fetchAllItems("/devices");
		for (const auto& item : items) {
			std::string id = json_utils::getValueOr<std::string>(item, "deviceId", "");
			if (id.empty()) {
				continue;
			}
			auto device = makeTrackedDevice(id);
			device->initFromJson(item);
			registerDevice(device);
			devices.push_back(std::move(device));
		}

		return devices;
	}

	std::shared_ptr<Device> Client::getDevice(const std::string& device_id) {
		if (!isAuthenticated() || device_id.empty()) {
			return nullptr;
		}

		try {
			auto response = getWithAuth(_base_url + "/devices/" + device_id);
			if (!response.isSuccess()) {
				return nullptr;
			}

			auto json_response = json_utils::parse(response.body);
			std::string id = json_utils::getValueOr<std::string>(json_response, "deviceId", device_id);

			auto device = makeTrackedDevice(id);
			device->initFromJson(json_response);
			registerDevice(device);
			return device;
		}
		catch (const std::exception&) {
			return nullptr;
		}
	}

	std::vector<Location> Client::getLocations() {
		std::vector<Location> locations;

		auto items = fetchAllItems("/locations");
		for (const auto& item : items) {
			Location location;
			location.locationId = json_utils::getValueOr<std::string>(item, "locationId", "");
			location.name = json_utils::getValueOr<std::string>(item, "name", "");
			location.countryCode = json_utils::getValueOr<std::string>(item, "countryCode", "");
			if (!location.locationId.empty()) {
				locations.push_back(std::move(location));
			}
		}

		return locations;
	}

	std::vector<Room> Client::getRooms(const std::string& location_id) {
		std::vector<Room> rooms;

		if (location_id.empty()) {
			return rooms;
		}

		auto items = fetchAllItems("/locations/" + location_id + "/rooms");
		for (const auto& item : items) {
			Room room;
			room.roomId = json_utils::getValueOr<std::string>(item, "roomId", "");
			room.locationId = json_utils::getValueOr<std::string>(item, "locationId", location_id);
			room.name = json_utils::getValueOr<std::string>(item, "name", "");
			if (!room.roomId.empty()) {
				rooms.push_back(std::move(room));
			}
		}

		return rooms;
	}

	nlohmann::json Client::getDeviceStatus(const std::string& device_id) {
		if (!isAuthenticated() || device_id.empty()) {
			return nlohmann::json(nullptr);
		}

		try {
			auto response = getWithAuth(_base_url + "/devices/" + device_id + "/status");
			if (!response.isSuccess()) {
				return nlohmann::json(nullptr);
			}
			return json_utils::parse(response.body);
		}
		catch (const std::exception&) {
			return nlohmann::json(nullptr);
		}
	}

	nlohmann::json Client::getCapabilityStatus(const std::string& device_id,
		const std::string& component_id, const std::string& capability_id) {
		if (!isAuthenticated() || device_id.empty() || component_id.empty() || capability_id.empty()) {
			return nlohmann::json(nullptr);
		}

		try {
			std::string url = _base_url + "/devices/" + device_id + "/components/" + component_id
				+ "/capabilities/" + capability_id + "/status";
			auto response = getWithAuth(url);
			if (!response.isSuccess()) {
				return nlohmann::json(nullptr);
			}
			return json_utils::parse(response.body);
		}
		catch (const std::exception&) {
			return nlohmann::json(nullptr);
		}
	}

	Result<void> Client::executeCommands(const std::string& device_id, const nlohmann::json& commands) {
		if (!isAuthenticated()) {
			return Result<void>(ErrorCode::AuthenticationRequired, "No access token set");
		}
		if (device_id.empty()) {
			return Result<void>(ErrorCode::InvalidParameter, "Device id is empty");
		}

		try {
			std::string body = commands.dump();
			auto response = postWithAuth(_base_url + "/devices/" + device_id + "/commands", body);

			if (response.isSuccess()) {
				// 200 OK or 207 Multi-Status: the command was accepted.
				return Result<void>();
			}

			switch (response.status_code) {
			case 401:
			case 403:
				return Result<void>(ErrorCode::AuthenticationFailed,
					"Access token is invalid or lacks the required scope (need x:devices:*)");
			case 404:
				return Result<void>(ErrorCode::ResourceNotFound, "Device not found");
			case 409:
				return Result<void>(ErrorCode::Conflict, "Device is offline or in a conflicting state");
			case 422:
				return Result<void>(ErrorCode::InvalidParameter,
					"Invalid command or arguments for this capability");
			case 429:
				return Result<void>(ErrorCode::RateLimited, "Rate limit exceeded");
			case 0:
				return Result<void>(ErrorCode::NetworkError,
					"Failed to reach the SmartThings API: " + response.error_message);
			default:
				return Result<void>(ErrorCode::ApiError,
					"Unexpected status " + std::to_string(response.status_code)
					+ " executing command");
			}
		}
		catch (const std::exception& e) {
			return Result<void>(ErrorCode::UnknownError,
				std::string("Command error: ") + e.what());
		}
	}

	void Client::registerDevice(const std::shared_ptr<Device>& device) {
		if (!device) {
			return;
		}

		{
			std::lock_guard<std::mutex> lock(_registryMutex);
			auto& bucket = _deviceRegistry[device->Id.Get()];

			// Opportunistic prune: drop expired entries for this id while the lock
			// is already held, so a Manual-mode Client that never calls pollNow()
			// (and so never hits snapshotRegistry()'s full prune) doesn't grow this
			// bucket unboundedly across repeated getDevice() calls for the same id.
			bucket.erase(
				std::remove_if(bucket.begin(), bucket.end(),
					[](const std::weak_ptr<Device>& w) { return w.expired(); }),
				bucket.end());

			bucket.push_back(device);
		}

		// Outside the registry lock - may perform a network call in OAuth mode.
		maybeSubscribeDevice(device->Id.Get());
	}

	std::shared_ptr<Device> Client::makeTrackedDevice(const std::string& id) {
		std::weak_ptr<DeviceLifetimeHub> weakHub = _lifetimeHub;
		return std::shared_ptr<Device>(new Device(id, this),
			[weakHub, id](Device* device) {
				delete device;
				if (auto hub = weakHub.lock()) {
					hub->notifyReleased(id);
				}
			});
	}

	void Client::onDeviceReleased(const std::string& deviceId) {
		bool lastGone = false;
		{
			std::lock_guard<std::mutex> lock(_registryMutex);
			auto it = _deviceRegistry.find(deviceId);
			if (it == _deviceRegistry.end()) {
				// Already swept (e.g. by snapshotRegistry): nothing lives.
				lastGone = true;
			}
			else {
				auto& bucket = it->second;
				bucket.erase(
					std::remove_if(bucket.begin(), bucket.end(),
						[](const std::weak_ptr<Device>& w) { return w.expired(); }),
					bucket.end());
				if (bucket.empty()) {
					_deviceRegistry.erase(it);
					lastGone = true;
				}
			}
		}
		if (!lastGone || !_isOAuth) {
			return;
		}

		// Last Device for this id is gone: its automatic subscription goes too.
		std::string subscriptionId;
		{
			std::lock_guard<std::mutex> lock(_subscriptionMutex);
			_pendingSubscriptions.erase(deviceId);
			auto it = _subscribedDevices.find(deviceId);
			if (it != _subscribedDevices.end()) {
				subscriptionId = it->second;
				_subscribedDevices.erase(it);
			}
		}
		if (!subscriptionId.empty()) {
			deleteSubscription(subscriptionId);
		}
	}

	std::vector<std::pair<std::string, std::vector<std::shared_ptr<Device>>>> Client::snapshotRegistry() {
		std::vector<std::pair<std::string, std::vector<std::shared_ptr<Device>>>> snapshot;

		std::lock_guard<std::mutex> lock(_registryMutex);
		for (auto it = _deviceRegistry.begin(); it != _deviceRegistry.end(); ) {
			auto& [id, weakDevices] = *it;

			std::vector<std::shared_ptr<Device>> live;
			live.reserve(weakDevices.size());
			for (auto& w : weakDevices) {
				if (auto locked = w.lock()) {
					live.push_back(std::move(locked));
				}
			}

			if (live.empty()) {
				// Nothing survived: this id has no live Device left, drop it
				// entirely rather than leaving an empty bucket behind.
				it = _deviceRegistry.erase(it);
				continue;
			}

			weakDevices.clear();
			for (auto& d : live) {
				weakDevices.push_back(d);
			}
			snapshot.emplace_back(id, std::move(live));
			++it;
		}

		return snapshot;
	}

	void Client::pollNow() {
		for (auto& [deviceId, devices] : snapshotRegistry()) {
			auto status = getDeviceStatus(deviceId);
			if (status.is_null() || !status.is_object()) {
				continue; // offline/rate-limited/error: retried next cycle
			}
			for (auto& device : devices) {
				device->applyStatus(status);
			}
		}
	}

	void Client::startPolling() {
		if (_pollingRunning) {
			return;
		}
		_pollingRunning = true;

		_pollingThread = std::thread([this]() {
			std::unique_lock<std::mutex> lock(_pollingMutex);
			while (_pollingRunning) {
				auto interval = _pollingInterval;
				lock.unlock();
				pollNow();
				lock.lock();
				_pollingCv.wait_for(lock, interval, [this]() { return !_pollingRunning.load(); });
			}
			});
	}

	void Client::stopPolling() {
		if (!_pollingRunning) {
			return;
		}

		{
			std::lock_guard<std::mutex> lock(_pollingMutex);
			_pollingRunning = false;
		}
		_pollingCv.notify_all();

		if (_pollingThread.joinable()) {
			_pollingThread.join();
		}
	}

	bool Client::isPolling() const {
		return _pollingRunning;
	}

	void Client::setPollingInterval(std::chrono::milliseconds interval) {
		std::lock_guard<std::mutex> lock(_pollingMutex);
		_pollingInterval = std::max(interval, MIN_POLLING_INTERVAL);
	}

	std::chrono::milliseconds Client::getPollingInterval() const {
		std::lock_guard<std::mutex> lock(_pollingMutex);
		return _pollingInterval;
	}

	// --- OAuth authentication flow ------------------------------------------

	Result<bool> Client::authenticate() {
		if (!_isOAuth) {
			return Result<bool>(ErrorCode::InvalidRequest,
				"authenticate() requires an OAuth-mode Client (see the OAuth constructors)");
		}
		if (!_serverStart.isSuccess()) {
			return Result<bool>(_serverStart.error, _serverStart.error_message);
		}

		if (auto stored = loadPersistedOAuthToken()) {
			// 1. Still-valid access token: nothing to do.
			if (!stored->accessToken.empty() && !stored->isExpired()) {
				{
					std::lock_guard<std::mutex> lock(_authMutex);
					_access_token = stored->accessToken;
				}
				const auto validateAuthenticationResult = validateAuthentication();

				if (validateAuthenticationResult.isSuccess()) {
					adoptInstalledAppId(stored->installedAppId);
					flushPendingSubscriptions();
					completeAuthentication(Result<void>());
					return Result<bool>(true);
				}
				_access_token = "";

				if (validateAuthenticationResult.error == ErrorCode::AuthenticationFailed) {
					stored = oauth2::OAuth2Token();
					applyOAuthToken(*stored);
				}
				else {
					Result<bool> res(validateAuthenticationResult.error, validateAuthenticationResult.error_message);
					res.value = false;
					return res;
				}
			}
			// 2. Refresh token: silent renewal (rotates + persists the pair).
			if (!stored->refreshToken.empty()) {
				auto refreshed = _authenticator->refresh(stored->refreshToken);
				if (refreshed.isSuccess() && refreshed.hasValue()) {
					applyOAuthToken(*refreshed.value);
					completeAuthentication(Result<void>());
					return Result<bool>(true);
				}
				if (refreshed.error == ErrorCode::NetworkError
					|| refreshed.error == ErrorCode::RateLimited) {
					// A new consent can't fix an unreachable token endpoint.
					return Result<bool>(refreshed.error, refreshed.error_message);
				}
				// Refresh token rejected (expired/revoked): fall through to consent.
			}
		}

		// 3. Interactive consent - the one step the library can't do itself.
		{
			std::lock_guard<std::mutex> lock(_authWaitMutex);
			_authFinished = false;
		}
		std::string url = _authenticator->buildAuthorizeUrl(_httpServer->fullOAuthCallbackUri());
		openBrowserRequested.Notify(url);
		return Result<bool>(false);
	}

	Result<void> Client::waitForAuthentication(std::chrono::milliseconds timeout) {
		if (!_isOAuth) {
			return Result<void>(ErrorCode::InvalidRequest,
				"waitForAuthentication() requires an OAuth-mode Client");
		}
		std::unique_lock<std::mutex> lock(_authWaitMutex);
		if (!_authWaitCv.wait_for(lock, timeout, [this]() { return _authFinished; })) {
			return Result<void>(ErrorCode::TimeoutError,
				"Authentication did not complete within the allotted time");
		}
		return _authOutcome;
	}

	void Client::onOAuthRedirect(std::string args) {
		auto exchanged = _authenticator->completeAuthorization(args);
		if (exchanged.isSuccess() && exchanged.hasValue()) {
			applyOAuthToken(*exchanged.value);
			completeAuthentication(Result<void>());
		}
		else {
			completeAuthentication(Result<void>(exchanged.error, exchanged.error_message));
		}
	}

	void Client::applyOAuthToken(const oauth2::OAuth2Token& token) {
		persistOAuthToken(token);
		{
			std::lock_guard<std::mutex> lock(_authMutex);
			_access_token = token.accessToken;
		}
		adoptInstalledAppId(token.installedAppId);
		flushPendingSubscriptions();
	}

	void Client::persistOAuthToken(const oauth2::OAuth2Token& token) const {
		if (!_keychain) {
			return;
		}
		oauth2::OAuth2Token merged = token;
		// A refresh response may omit fields the original grant carried; keep
		// the previous values rather than losing them.
		if (merged.refreshToken.empty() || merged.installedAppId.empty()) {
			if (auto previous = loadPersistedOAuthToken()) {
				if (merged.refreshToken.empty()) {
					merged.refreshToken = previous->refreshToken;
				}
				if (merged.installedAppId.empty()) {
					merged.installedAppId = previous->installedAppId;
				}
			}
		}
		nlohmann::json blob = {
			{ "accessToken", merged.accessToken },
			{ "refreshToken", merged.refreshToken },
			{ "installedAppId", merged.installedAppId },
			{ "scope", merged.scope },
			{ "issuedAtEpochSeconds", std::chrono::duration_cast<std::chrono::seconds>(
				merged.issuedAt.time_since_epoch()).count() },
			{ "expiresInSeconds", merged.expiresIn.count() }
		};
		_keychain->setValue("oauth.token", blob.dump());
	}

	std::optional<oauth2::OAuth2Token> Client::loadPersistedOAuthToken() const {
		if (!_keychain) {
			return std::nullopt;
		}
		auto raw = _keychain->getValue("oauth.token");
		if (!raw) {
			return std::nullopt;
		}
		try {
			auto blob = json_utils::parse(*raw);
			oauth2::OAuth2Token token;
			token.accessToken = json_utils::getValueOr<std::string>(blob, "accessToken", "");
			token.refreshToken = json_utils::getValueOr<std::string>(blob, "refreshToken", "");
			token.installedAppId = json_utils::getValueOr<std::string>(blob, "installedAppId", "");
			token.scope = json_utils::getValueOr<std::string>(blob, "scope", "");
			token.issuedAt = std::chrono::system_clock::time_point{}
			+ std::chrono::seconds(json_utils::getValueOr<long long>(blob, "issuedAtEpochSeconds", 0));
			token.expiresIn = std::chrono::seconds(
				json_utils::getValueOr<long long>(blob, "expiresInSeconds", 0));
			if (token.accessToken.empty() && token.refreshToken.empty()) {
				return std::nullopt;
			}
			return token;
		}
		catch (const std::exception&) {
			return std::nullopt;
		}
	}

	void Client::adoptInstalledAppId(const std::string& installedAppId) {
		if (installedAppId.empty()) {
			return;
		}
		{
			std::lock_guard<std::mutex> lock(_installMutex);
			_installedAppId = installedAppId;
		}
		if (_storage) {
			_storage->setValue("installedAppId", installedAppId);
		}
	}

	void Client::completeAuthentication(const Result<void>& outcome) {
		{
			std::lock_guard<std::mutex> lock(_authWaitMutex);
			_authOutcome = outcome;
			_authFinished = true;
		}
		_authWaitCv.notify_all();
		authenticationCompleted.Notify(outcome);
	}

} // namespace smartthings4cpp
