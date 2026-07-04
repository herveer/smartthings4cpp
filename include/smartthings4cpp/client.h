#pragma once

#include "types.h"
#include "webhook.h"
#include "http_server.h"
#include "storage.h"
#include "oauth2/oauth2_types.h"
#include <ReactiveLitepp/Event.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
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

	// Forward declaration (full type only needed in client.cpp).
	namespace oauth2 { class OAuth2Authenticator; }

	/**
	 * @brief Main entry point for interacting with a SmartThings account
	 *
	 * Unlike a local Hue bridge, SmartThings is a cloud service: there is no
	 * network discovery and no link-button pairing. The Client talks to
	 * https://api.smartthings.com/v1 and comes in two flavors, chosen by
	 * constructor:
	 *
	 * - **PAT mode** - construct with a Personal Access Token. Live updates
	 *   come from automatic background polling (see PollingMode), since a PAT
	 *   cannot subscribe to events.
	 * - **OAuth mode** - construct with an OAuth-In App's OAuth2Config. The
	 *   Client runs the authorization flow itself (see authenticate() /
	 *   openBrowserRequested), persists tokens across runs, receives real push
	 *   events on an embedded (or injected, see IHttpServer) local HTTP server,
	 *   and manages device event subscriptions automatically - each Device it
	 *   hands out is subscribed on creation and unsubscribed when the last
	 *   Device object for that id is destroyed. No polling.
	 *
	 * In both modes the Client discovers devices (with their components and
	 * capabilities), locations and rooms, and updates arrive through the same
	 * reactive PropertyChanged events.
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
		 * @brief Construct an OAuth Client with everything embedded (recommended)
		 *
		 * Runs the whole OAuth + events story autonomously: an embedded local
		 * HTTP server handles both the OAuth redirect and the SmartThings
		 * webhook, tokens persist across runs (Windows Credential Manager /
		 * file storage - see storage.h), and device event subscriptions are
		 * created and deleted automatically as Device objects come and go. The
		 * one thing the library never does itself is open a browser: subscribe
		 * to openBrowserRequested and present the URL, then call authenticate().
		 *
		 * The webhook route must be reachable from the SmartThings cloud over
		 * HTTPS: front the local port with a tunnel (e.g. @c ngrok @c http
		 * @c <port>) and register that public URL as the app's Target URL. The
		 * OAuth redirect works directly against
		 * @c http://localhost:<port><oauthCallbackRoute>, which must match a
		 * redirect URI registered on the app.
		 *
		 * No background polling in OAuth mode - webhook events drive updates
		 * (startPolling() remains available as a manual opt-in).
		 * @param config Your OAuth-In App's clientId/clientSecret/scopes
		 * @param port Local port for the embedded server (0 = pick a free one)
		 * @param oauthCallbackRoute Path of the OAuth redirect route, e.g. "/oauth/callback"
		 * @param webhookCallbackRoute Path of the webhook route, e.g. "/webhook"
		 * @param externalUri The public HTTPS URL of the oAuth and webhook routes, e.g. "https://<tunnel>/webhook"
		 * @param workingDir Persistent directory for the default storage
		 *        (created if missing; also the token fallback store on
		 *        platforms without a native keychain backend)
		 */
		Client(oauth2::OAuth2Config config, int port, std::string oauthCallbackRoute,
			std::string webhookCallbackRoute, std::string externalUri, std::filesystem::path workingDir);

		/**
		 * @brief Construct an OAuth Client from injected implementations
		 *
		 * Same autonomous behavior as the embedded-server constructor, but you
		 * supply the pieces: an IHttpServer bridging your app's own HTTP stack,
		 * and IStorageProvider implementations for secrets (tokens) and plain
		 * state. Client binds the server's callbacks and starts it.
		 * @param config Your OAuth-In App's clientId/clientSecret/scopes
		 * @param httpServer Receives the OAuth redirect + webhook requests
		 * @param keychainProvider Secret storage (the OAuth token pair)
		 * @param storageProvider Plain state storage (installed-app id)
		 */
		Client(oauth2::OAuth2Config config, std::unique_ptr<IHttpServer> httpServer,
			std::unique_ptr<IStorageProvider> keychainProvider,
			std::unique_ptr<IStorageProvider> storageProvider);

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

		// --- OAuth: authentication + events ---------------------------------
		// Real push, in place of polling, for an OAuth "API Access" app. With
		// an OAuth-mode Client everything below runs automatically: the
		// embedded (or injected) IHttpServer receives the OAuth redirect and
		// the webhook, tokens persist across runs, and device subscriptions
		// are created/deleted with the Device objects themselves - events fire
		// the same PropertyChanged polling would. Typical use is just:
		//   client.openBrowserRequested += [](std::string url) { /* open it */ };
		//   client.authenticate();
		//   client.waitForAuthentication(std::chrono::minutes(5));
		// The subscription/webhook methods further below are escape hatches -
		// normal consumers never call them.

		/**
		 * @brief Fired when the user's consent is needed: present this URL
		 *
		 * The one manual step of the OAuth flow. Fired by authenticate() when
		 * no stored token can be (re)used; open the URL in a browser (or show
		 * it to the user). Once the redirect lands on the embedded/injected
		 * server, authentication finishes by itself - observe
		 * authenticationCompleted or block in waitForAuthentication().
		 */
		ReactiveLitepp::Event<std::string> openBrowserRequested;

		/**
		 * @brief Fired when an authentication attempt finishes
		 *
		 * Success after the browser round-trip (or a silent token refresh);
		 * an error Result if the exchange/refresh failed (e.g. consent denied,
		 * CSRF state mismatch, network failure).
		 * @note Fires on the HTTP server's thread for the browser flow.
		 */
		ReactiveLitepp::Event<Result<void>> authenticationCompleted;

		/**
		 * @brief Make this OAuth Client authenticated, as silently as possible
		 *
		 * Tries, in order: the stored access token (if not expired), the stored
		 * refresh token (rotating + persisting the new pair), and finally the
		 * interactive flow - firing openBrowserRequested and returning
		 * immediately. Call once at startup; tokens persisting across runs
		 * means the browser typically only opens the very first time.
		 * @return On success, @c true if authenticated right now, @c false if
		 *         the browser step is pending (wait via waitForAuthentication()
		 *         or authenticationCompleted). An error Result if this isn't an
		 *         OAuth Client, the server failed to start, or a refresh failed
		 *         for a reason consent can't fix (e.g. network down).
		 */
		Result<bool> authenticate();

		/**
		 * @brief Block until the pending authentication finishes (or times out)
		 * @param timeout Maximum time to wait for the browser round-trip
		 * @return The outcome (TimeoutError if nothing completed in time)
		 */
		Result<void> waitForAuthentication(std::chrono::milliseconds timeout);

		/**
		 * @brief Set the installed-app id that subscription calls target
		 *
		 * Normally learned automatically from the Install/Update messageType by
		 * handleWebhook(); set it explicitly if you manage installation yourself.
		 */
		void setInstalledAppId(const std::string& installedAppId);

		/** @brief The installed-app id in use (empty until known). */
		std::string installedAppId() const;

		/**
		 * @brief Subscribe to a device's (or capability's) events
		 *
		 * POST /installedapps/{installedAppId}/subscriptions. Requires an
		 * OAuth-mode Client and a known installedAppId(). Escape hatch: device
		 * subscriptions are normally created automatically (see the class doc);
		 * use this only for narrower-than-default subscriptions.
		 * @param request What to subscribe to (see SubscriptionRequest)
		 * @return The created Subscription, or an error Result
		 */
		Result<Subscription> subscribeTo(const SubscriptionRequest& request);

		/**
		 * @brief Subscribe to every currently-registered device (all capabilities)
		 *
		 * Convenience that issues one subscription per unique registered device.
		 * Escape hatch - the same thing already happens automatically as devices
		 * are created.
		 * @return The number of subscriptions created, or an error Result
		 */
		Result<int> subscribeToAllDevices();

		/**
		 * @brief List the installed-app's current subscriptions
		 * @return The subscriptions, or an error Result
		 */
		Result<std::vector<Subscription>> listSubscriptions();

		/**
		 * @brief Delete a single subscription by id
		 * @param subscriptionId The Subscription::id to remove
		 */
		Result<void> deleteSubscription(const std::string& subscriptionId);

		/** @brief Delete every subscription for the installed app. */
		Result<void> deleteAllSubscriptions();

		/**
		 * @brief Process one webhook messageType request from SmartThings
		 *
		 * Called automatically by the embedded/injected IHttpServer for every
		 * POST on the webhook route - exposed (public-but-internal, like
		 * registerDevice()) for tests and for apps that receive the POST some
		 * other way. Answers the PING/CONFIRMATION handshakes, captures the
		 * installed-app id + token on Install/Update (then flushes any pending
		 * automatic subscriptions), and on an EVENT dispatches each device
		 * event onto every live Device for that id - firing PropertyChanged
		 * just like a poll would. The returned WebhookResponse is what must go
		 * back to SmartThings (status + JSON body, application/json).
		 * @param rawBody The exact request body SmartThings POSTed
		 * @return The HTTP response to return, plus which messageType was handled
		 * @note This iteration does NOT cryptographically verify the request
		 *       signature; keep the endpoint behind a trusted tunnel. Signature
		 *       verification is a planned follow-up.
		 */
		WebhookResponse handleWebhook(const std::string& rawBody);

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

		// OAuth-mode collaborators (all null on a PAT Client). The server runs
		// its own thread and calls back into this Client (onOAuthRedirect /
		// handleWebhook), so ~Client stops it before tearing anything else down.
		bool _isOAuth = false;
		oauth2::OAuth2Config _oauthConfig;
		std::unique_ptr<oauth2::OAuth2Authenticator> _authenticator;
		std::unique_ptr<IHttpServer> _httpServer;
		std::unique_ptr<IStorageProvider> _keychain; // secrets: the OAuth token pair
		std::unique_ptr<IStorageProvider> _storage;  // plain state: installed-app id
		Result<void> _serverStart{ ErrorCode::InvalidRequest, "not an OAuth client" };

		// waitForAuthentication() rendezvous with the async browser round-trip.
		mutable std::mutex _authWaitMutex;
		std::condition_variable _authWaitCv;
		bool _authFinished = false;              // guarded by _authWaitMutex
		Result<void> _authOutcome;               // guarded by _authWaitMutex

		// Installed-app identity, learned from the token response (SmartThings
		// includes installed_app_id) or the Install/Update messageType; guarded by
		// _installMutex because handleWebhook() runs on the server thread.
		mutable std::mutex _installMutex;
		std::string _installedAppId;      // guarded by _installMutex
		std::string _installedAppToken;   // guarded by _installMutex

		// Automatic per-device subscriptions (OAuth mode). All guarded by
		// _subscriptionMutex; network calls always happen with it released.
		mutable std::mutex _subscriptionMutex;
		std::unordered_map<std::string, std::string> _subscribedDevices;   // deviceId -> subscriptionId
		std::unordered_set<std::string> _pendingSubscriptions;             // waiting for auth/appId/429-retry
		std::unordered_set<std::string> _subscriptionsInFlight;            // create in progress
		std::unordered_map<std::string, std::string> _unclaimedSubscriptions; // prior-run leftovers to adopt
		bool _unclaimedLoaded = false;

		// Lets the shared_ptr<Device> custom deleter reach back into this Client
		// safely: the deleter holds a weak_ptr to this hub, and ~Client disarms
		// it, so a Device destroyed after its Client is a silent no-op instead
		// of a dangling call.
		struct DeviceLifetimeHub;
		std::shared_ptr<DeviceLifetimeHub> _lifetimeHub;

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

		/// Under _registryMutex: lock (and prune) the registry bucket for one
		/// device id, returning its currently-live Device objects. Used to fan a
		/// single webhook device event out to every Device wrapping that id.
		std::vector<std::shared_ptr<Device>> liveDevicesFor(const std::string& deviceId);

		/// Token used for /installedapps subscription calls: the installed-app
		/// token captured from the messageType if present, else the access token.
		std::string subscriptionToken() const;

		/// Create a registry-tracked Device whose destruction (of the last
		/// instance for its id) tears its automatic subscription down via
		/// _lifetimeHub. Used by getDevices()/getDevice().
		std::shared_ptr<Device> makeTrackedDevice(const std::string& id);

		/// Called (via _lifetimeHub) when a tracked Device is destroyed: prunes
		/// the registry and, if that was the last Device for the id, deletes its
		/// automatic subscription.
		void onDeviceReleased(const std::string& deviceId);

		/// OAuth redirect landing (server thread): exchanges the code, persists
		/// the token, flushes pending subscriptions, completes authentication.
		void onOAuthRedirect(std::string args);

		/// Persist + adopt a freshly-obtained token pair: keychain write,
		/// _access_token update, installed-app id capture, pending-subscription
		/// flush. NOT for use inside getWithAuth()'s 401 path (locks _authMutex).
		void applyOAuthToken(const oauth2::OAuth2Token& token);

		/// Keychain-only write of the token blob (merges the previous refresh
		/// token / installed-app id when the new token omits them). Safe under
		/// _authMutex - touches no Client mutex itself.
		void persistOAuthToken(const oauth2::OAuth2Token& token) const;

		/// Keychain-only read of the persisted token blob.
		std::optional<oauth2::OAuth2Token> loadPersistedOAuthToken() const;

		/// Record the installed-app id (in memory + plain storage) if non-empty.
		void adoptInstalledAppId(const std::string& installedAppId);

		/// Record an authentication outcome: wakes waitForAuthentication() and
		/// fires authenticationCompleted.
		void completeAuthentication(const Result<void>& outcome);

		/// Ensure the given device id has an automatic subscription (OAuth mode):
		/// adopt a prior run's, create one, or park it as pending until
		/// authentication/installed-app id/rate-limit allows.
		void maybeSubscribeDevice(const std::string& deviceId);

		/// Retry every pending automatic subscription (called after
		/// authentication completes and after Install/Update lifecycles).
		void flushPendingSubscriptions();

		/// One-shot listSubscriptions() to adopt prior-run subscriptions instead
		/// of re-creating them (subscription writes are rate-limited).
		void loadUnclaimedSubscriptionsOnce();
	};

} // namespace smartthings4cpp
