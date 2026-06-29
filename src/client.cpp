#include "smartthings4cpp/client.h"
#include "smartthings4cpp/device.h"
#include "smartthings4cpp/http_client.h"
#include "smartthings4cpp/json_utils.h"

#include <nlohmann/json.hpp>
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

	Client::Client() = default;

	Client::Client(const std::string& access_token)
		: _access_token(access_token) {
	}

	Client::~Client() = default;

	Client::Client(Client&& other) noexcept
		: _access_token(std::move(other._access_token)),
		_base_url(std::move(other._base_url)) {
	}

	Client& Client::operator=(Client&& other) noexcept {
		if (this != &other) {
			_access_token = std::move(other._access_token);
			_base_url = std::move(other._base_url);
		}
		return *this;
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

	void Client::setBaseUrl(const std::string& url) {
		_base_url = url;
	}

	const std::string& Client::getBaseUrl() const {
		return _base_url;
	}

	std::map<std::string, std::string> Client::authHeaders() const {
		return {
			{ "Authorization", "Bearer " + _access_token },
			{ "Accept", "application/json" }
		};
	}

	Result<void> Client::validateAuthentication() const {
		if (_access_token.empty()) {
			return Result<void>(ErrorCode::AuthenticationRequired,
				"No access token set");
		}

		try {
			HttpClient client;
			client.setTimeout(REQUEST_TIMEOUT);

			auto response = client.get(_base_url + "/devices?max=1", authHeaders());

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
			HttpClient client;
			client.setTimeout(REQUEST_TIMEOUT);

			std::string url = _base_url + path;
			auto headers = authHeaders();

			// Follow _links.next until the API stops handing out pages. The bound
			// is a safety net against a misbehaving server returning a cycle.
			for (int page = 0; page < 1000 && !url.empty(); ++page) {
				auto response = client.get(url, headers);
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

	std::vector<std::unique_ptr<Device>> Client::getDevices() {
		std::vector<std::unique_ptr<Device>> devices;

		auto items = fetchAllItems("/devices");
		for (const auto& item : items) {
			std::string id = json_utils::getValueOr<std::string>(item, "deviceId", "");
			if (id.empty()) {
				continue;
			}
			auto device = std::make_unique<Device>(id, this);
			device->initFromJson(item);
			devices.push_back(std::move(device));
		}

		return devices;
	}

	std::unique_ptr<Device> Client::getDevice(const std::string& device_id) {
		if (!isAuthenticated() || device_id.empty()) {
			return nullptr;
		}

		try {
			HttpClient client;
			client.setTimeout(REQUEST_TIMEOUT);

			auto response = client.get(_base_url + "/devices/" + device_id, authHeaders());
			if (!response.isSuccess()) {
				return nullptr;
			}

			auto json_response = json_utils::parse(response.body);
			std::string id = json_utils::getValueOr<std::string>(json_response, "deviceId", device_id);

			auto device = std::make_unique<Device>(id, this);
			device->initFromJson(json_response);
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

} // namespace smartthings4cpp
