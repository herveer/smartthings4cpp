#pragma once

#include "types.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <nlohmann/json_fwd.hpp>

/**
 * @file client.h
 * @brief SmartThings cloud client: authentication and resource discovery
 */

namespace smartthings4cpp {

	// Forward declarations
	class Device;

	/**
	 * @brief Main entry point for interacting with a SmartThings account
	 *
	 * Unlike a local Hue bridge, SmartThings is a cloud service: there is no
	 * network discovery and no link-button pairing. Instead the Client is
	 * configured with an OAuth 2.0 Bearer token (a Personal Access Token, PAT,
	 * for this iteration) and talks to https://api.smartthings.com/v1.
	 *
	 * Once authenticated, the Client discovers devices (with their components and
	 * capabilities), locations and rooms.
	 *
	 * @note Sending commands and reading live device status will be added in the
	 *       device-control iteration via this same Client.
	 */
	class Client {
	public:
		/// Default base URL for the SmartThings public API.
		static constexpr const char* DEFAULT_BASE_URL = "https://api.smartthings.com/v1";

		/**
		 * @brief Default constructor (no token set)
		 */
		Client();

		/**
		 * @brief Construct a Client with an access token
		 * @param access_token A SmartThings OAuth 2.0 Bearer token (PAT)
		 */
		explicit Client(const std::string& access_token);

		/**
		 * @brief Destructor
		 */
		~Client();

		// Prevent copying, allow moving
		Client(const Client&) = delete;
		Client& operator=(const Client&) = delete;
		Client(Client&&) noexcept;
		Client& operator=(Client&&) noexcept;

		/**
		 * @brief Set the access token used to authenticate API requests
		 * @param token A SmartThings OAuth 2.0 Bearer token (PAT)
		 */
		void setAccessToken(const std::string& token);

		/**
		 * @brief Get the currently configured access token
		 * @return The access token, or an empty string if none is set
		 */
		std::string getAccessToken() const;

		/**
		 * @brief Check whether an access token has been configured
		 * @return true if a non-empty token is set, false otherwise
		 * @note This does not contact the API; use validateAuthentication() for that.
		 */
		bool isAuthenticated() const;

		/**
		 * @brief Validate the configured token against the SmartThings API
		 *
		 * Performs a lightweight authenticated request (GET /v1/devices?max=1) and
		 * inspects the HTTP status: 2xx means the token is valid, 401/403 means it
		 * is invalid or lacks the required scope.
		 * @return Result indicating whether the token is valid
		 */
		Result<void> validateAuthentication() const;

		/**
		 * @brief Get all devices in the account
		 *
		 * Walks the paginated GET /v1/devices endpoint, following the
		 * @c _links.next links until every page is consumed.
		 * @return Vector of Device objects (empty on auth/network/parse failure)
		 */
		std::vector<std::unique_ptr<Device>> getDevices();

		/**
		 * @brief Get a single device by id
		 * @param device_id The unique identifier of the device (deviceId)
		 * @return Unique pointer to a Device, or nullptr if not found
		 */
		std::unique_ptr<Device> getDevice(const std::string& device_id);

		/**
		 * @brief Get all locations in the account
		 * @return Vector of Location structures (empty on failure)
		 */
		std::vector<Location> getLocations();

		/**
		 * @brief Get all rooms within a location
		 * @param location_id The location whose rooms to fetch
		 * @return Vector of Room structures (empty on failure)
		 */
		std::vector<Room> getRooms(const std::string& location_id);

		/**
		 * @brief Override the API base URL (mainly for testing / mocking)
		 * @param url Base URL without a trailing slash (e.g. the default
		 *            https://api.smartthings.com/v1)
		 */
		void setBaseUrl(const std::string& url);

		/**
		 * @brief Get the configured API base URL
		 * @return The base URL
		 */
		const std::string& getBaseUrl() const;

	private:
		std::string _access_token;
		std::string _base_url = DEFAULT_BASE_URL;

		/// Build the Authorization (and Accept) headers for an authenticated request.
		std::map<std::string, std::string> authHeaders() const;

		/// GET a paginated collection endpoint and accumulate every page's "items"
		/// array. @p path is relative to the base URL (e.g. "/devices").
		/// Returns an empty array on any auth/network/parse error.
		nlohmann::json fetchAllItems(const std::string& path) const;
	};

} // namespace smartthings4cpp
