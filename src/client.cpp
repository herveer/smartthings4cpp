#include "smartthings4cpp/client.h"
#include "smartthings4cpp/device.h"
#include "smartthings4cpp/http_client.h"
#include "smartthings4cpp/json_utils.h"

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

	Client::Client(PollingMode pollingMode) {
		if (pollingMode == PollingMode::Automatic) {
			startPolling();
		}
	}

	Client::Client(const std::string& access_token, PollingMode pollingMode)
		: _access_token(access_token) {
		if (pollingMode == PollingMode::Automatic) {
			startPolling();
		}
	}

	Client::~Client() {
		stopPolling();
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
			auto device = std::make_shared<Device>(id, this);
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

			auto device = std::make_shared<Device>(id, this);
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

} // namespace smartthings4cpp
