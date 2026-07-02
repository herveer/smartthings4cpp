#pragma once

#include "types.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

/**
 * @file client.h
 * @brief SmartThings cloud client: authentication and resource discovery
 */

namespace smartthings4cpp {

	// Forward declarations
	class Device;
	struct HttpResponse;

	/**
	 * @brief Controls whether a Client automatically polls its devices for live updates
	 *
	 * SmartThings does not push updates to a Personal-Access-Token client; the
	 * only way to observe a device's state changing is to re-fetch it. Automatic
	 * mode makes that transparent: Client runs a background thread that
	 * periodically re-fetches the status of every Device it has handed out and
	 * relies on the existing reactive property system (PropertyChanged fires
	 * only when a value actually changes) to surface the update.
	 * @note Once polling is active - the default - PropertyChanged fires on the
	 *       background polling thread. Device/Capability attribute storage is
	 *       not synchronized against concurrent access, so if you read
	 *       properties (or touch thread-affine state such as a GUI) from
	 *       another thread, you are responsible for synchronizing that yourself.
	 */
	enum class PollingMode {
		Automatic, ///< Client polls its devices in the background (default)
		Manual     ///< No background polling; call Client::pollNow() yourself
	};

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
		 * @param pollingMode Whether to start background polling immediately
		 *        (see PollingMode). Defaults to PollingMode::Automatic.
		 */
		explicit Client(PollingMode pollingMode = PollingMode::Automatic);

		/**
		 * @brief Construct a Client with an access token
		 * @param access_token A SmartThings OAuth 2.0 Bearer token (PAT)
		 * @param pollingMode Whether to start background polling immediately
		 *        (see PollingMode). Defaults to PollingMode::Automatic.
		 */
		explicit Client(const std::string& access_token, PollingMode pollingMode = PollingMode::Automatic);

		/**
		 * @brief Destructor
		 *
		 * Stops the background polling thread (if running) before the rest of
		 * the Client is torn down.
		 */
		~Client();

		/**
		 * @note Non-copyable and non-movable: Client owns a background polling
		 *       thread that captures `this`. Relocating a Client while it exists
		 *       would leave that thread operating on freed memory. Keep Client
		 *       instances in place - e.g. behind a std::unique_ptr<Client> if you
		 *       need heap allocation or container storage - rather than
		 *       moving/copying them.
		 */
		Client(const Client&) = delete;
		Client& operator=(const Client&) = delete;
		Client(Client&&) = delete;
		Client& operator=(Client&&) = delete;

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
		 * @brief Configure transparent token refresh on authentication expiry
		 *
		 * When set, any request that gets back HTTP 401 invokes @p callback
		 * once to obtain a fresh access token, then retries that single request
		 * before giving up. This is how OAuth2 access-token expiry (see
		 * smartthings4cpp::oauth2::OAuth2Authenticator) is made as transparent
		 * to callers as a Personal Access Token, which never expires mid-session.
		 * Optional: a PAT-only Client that never calls this behaves exactly as
		 * before (a 401 is simply returned to the caller, as today).
		 * @param callback Returns a new access token, or an error Result if the
		 *        refresh itself failed (e.g. the refresh token is also expired)
		 */
		void setTokenRefreshCallback(std::function<Result<std::string>()> callback);

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
		 * @c _links.next links until every page is consumed. Each returned
		 * Device is automatically registered for background polling (see
		 * PollingMode) - not identity-cached, so calling this (or getDevice())
		 * more than once for the same device id builds independent Device
		 * objects, but polling still fetches that id's status only once per
		 * cycle and applies it to every one of them.
		 * @return Vector of Device objects (empty on auth/network/parse failure)
		 */
		std::vector<std::shared_ptr<Device>> getDevices();

		/**
		 * @brief Get a single device by id
		 *
		 * Automatically registered for background polling, same as getDevices().
		 * @param device_id The unique identifier of the device (deviceId)
		 * @return Shared pointer to a Device, or nullptr if not found
		 */
		std::shared_ptr<Device> getDevice(const std::string& device_id);

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
		 * @brief Get the full status of a device (all components/capabilities/attributes)
		 *
		 * GET /v1/devices/{deviceId}/status.
		 * @param device_id The device whose status to fetch
		 * @return The parsed status JSON (shape @c {"components":{...}}), or a null
		 *         json on auth/network/parse failure
		 */
		nlohmann::json getDeviceStatus(const std::string& device_id);

		/**
		 * @brief Get the status of a single capability on a component
		 *
		 * GET /v1/devices/{deviceId}/components/{componentId}/capabilities/{capabilityId}/status.
		 * @return The per-capability status object @c {"<attr>":{"value":...}}, or a
		 *         null json on failure
		 */
		nlohmann::json getCapabilityStatus(const std::string& device_id,
			const std::string& component_id, const std::string& capability_id);

		/**
		 * @brief Execute one or more commands on a device
		 *
		 * POST /v1/devices/{deviceId}/commands.
		 * @param device_id The target device
		 * @param commands The command request body, e.g. produced by buildCommandsBody()
		 * @return Result indicating success or failure (mapped from the HTTP status)
		 */
		Result<void> executeCommands(const std::string& device_id, const nlohmann::json& commands);

		/// Floor enforced by setPollingInterval(): SmartThings caps device status
		/// reads at 12 requests/min *per device*; polling touches each unique
		/// device once per cycle regardless of how many are watched, so the
		/// constraint is on the interval itself (comfortably satisfied at 5s).
		static constexpr std::chrono::milliseconds MIN_POLLING_INTERVAL{ 5000 };

		/// Interval used when PollingMode::Automatic starts polling, until
		/// changed via setPollingInterval().
		static constexpr std::chrono::milliseconds DEFAULT_POLLING_INTERVAL{ 10000 };

		/**
		 * @brief Poll every currently-registered device once, synchronously
		 *
		 * Fetches each *unique* device id's status once - deduplicating Device
		 * objects that share the same underlying SmartThings device, e.g. from
		 * repeated getDevice() calls - and applies it to every still-alive
		 * Device for that id. Safe to call regardless of PollingMode: this is
		 * what the background thread calls on a timer in PollingMode::Automatic,
		 * and it's also how a PollingMode::Manual Client drives updates on the
		 * caller's own schedule.
		 */
		void pollNow();

		/**
		 * @brief Change the interval between automatic poll cycles
		 *
		 * Takes effect starting the next cycle; does not interrupt one already
		 * in-flight or waiting. Clamped up to MIN_POLLING_INTERVAL.
		 * @param interval Desired delay between poll cycles
		 */
		void setPollingInterval(std::chrono::milliseconds interval);

		/// Currently configured interval between automatic poll cycles.
		std::chrono::milliseconds getPollingInterval() const;

		/// Whether the background polling thread is currently running.
		bool isPolling() const;

		/**
		 * @brief Start the background polling thread if not already running
		 *
		 * No-op if PollingMode::Automatic already started it at construction,
		 * or if already running for any other reason. Lets a PollingMode::Manual
		 * Client (or one where stopPolling() was called) opt back in without
		 * reconstruction.
		 */
		void startPolling();

		/// Stop the background polling thread if running. Safe to call when not
		/// running. Blocks for at most one in-flight pollNow() call.
		void stopPolling();

		/**
		 * @brief Register a Device for background polling
		 *
		 * Normally called automatically by getDevices()/getDevice() - exposed so
		 * a Device obtained outside the normal flow (e.g. hand-constructed) can
		 * still be included in polling.
		 * @param device The device to register; ignored if null
		 */
		void registerDevice(const std::shared_ptr<Device>& device);

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
		// mutable: getWithAuth()/postWithAuth() transparently refresh this on a
		// 401 even when called from a const method (e.g. validateAuthentication),
		// the same way a lazily-refreshed cache field would be.
		mutable std::string _access_token;
		std::string _base_url = DEFAULT_BASE_URL;
		std::function<Result<std::string>()> _token_refresh_callback;

		// Serializes _access_token reads/writes across the token-refresh path.
		// Needed once a background poller and foreground calls can both hit a
		// 401 for the same expired token concurrently - see getWithAuth().
		mutable std::mutex _authMutex;

		// deviceId -> every currently-live Device wrapping that id, across every
		// getDevices()/getDevice()/registerDevice() call this Client has ever
		// served. weak_ptr so a destroyed Device silently and automatically
		// drops out (pruned in registerDevice() and snapshotRegistry()) with no
		// explicit unregister call required.
		mutable std::mutex _registryMutex;
		std::unordered_map<std::string, std::vector<std::weak_ptr<Device>>> _deviceRegistry;

		std::thread _pollingThread;
		mutable std::mutex _pollingMutex; // mutable: getPollingInterval() const locks it
		std::condition_variable _pollingCv;
		std::atomic<bool> _pollingRunning{ false };
		std::chrono::milliseconds _pollingInterval{ DEFAULT_POLLING_INTERVAL }; // guarded by _pollingMutex

		/// Build the Authorization (and Accept) headers for an authenticated
		/// request using an explicitly-supplied token (never reads _access_token
		/// directly, so callers must have already safely obtained one - see
		/// getWithAuth()/postWithAuth()).
		std::map<std::string, std::string> authHeadersFor(const std::string& token) const;

		/// GET a paginated collection endpoint and accumulate every page's "items"
		/// array. @p path is relative to the base URL (e.g. "/devices").
		/// Returns an empty array on any auth/network/parse error.
		nlohmann::json fetchAllItems(const std::string& path) const;

		/// Perform an authenticated GET. On a 401, if a token-refresh callback is
		/// configured, invokes it once and retries this same request once before
		/// returning. Every authenticated GET in this class routes through here
		/// so the retry is centralized instead of duplicated per method.
		HttpResponse getWithAuth(const std::string& url) const;

		/// POST equivalent of getWithAuth().
		HttpResponse postWithAuth(const std::string& url, const std::string& body) const;

		/// Under _registryMutex: locks every weak_ptr in the registry, pruning
		/// (and dropping empty buckets for) any device id with no survivors, and
		/// returns {deviceId, live devices} pairs for pollNow() to act on outside
		/// the lock.
		std::vector<std::pair<std::string, std::vector<std::shared_ptr<Device>>>> snapshotRegistry();
	};

} // namespace smartthings4cpp
