#pragma once

#include "types.h"
#include "webhook.h"
#include "webhook_signature.h"
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
	 * @brief Tag selecting the "external tokens" OAuth mode of Client
	 *
	 * Pass @c ExternalTokens{} as the first argument of the matching Client
	 * constructor to get an OAuth-mode Client whose tokens are supplied by the
	 * application (see Client::setAccessToken()) instead of being obtained,
	 * stored and refreshed by the library.
	 */
	struct ExternalTokens {
		explicit ExternalTokens() = default;
	};

	/**
	 * @brief Main entry point for interacting with a SmartThings account
	 *
	 * Unlike a local Hue bridge, SmartThings is a cloud service: there is no
	 * network discovery and no link-button pairing. The Client talks to
	 * https://api.smartthings.com/v1 and comes in three flavors, chosen by
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
	 * - **External-tokens OAuth mode** - construct with ExternalTokens{}. The
	 *   same push events and automatic subscriptions as OAuth mode, but the
	 *   application runs the OAuth flow itself and hands each access token in
	 *   with setAccessToken(); the library never stores, refreshes or waits
	 *   for a token, and reports a rejected one through accessTokenRejected.
	 *
	 * In every mode the Client discovers devices (with their components and
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
		 * @brief Construct an OAuth Client whose tokens come from the application
		 *
		 * For apps that already run OAuth themselves (build the authorize URL,
		 * receive the redirect, exchange the code, keep and refresh tokens). The
		 * Client does everything else an OAuth-mode Client does: it handles the
		 * webhook (PING/CONFIRMATION/EVENT/INSTALL/...), verifies each request's
		 * HTTP-Signature (the default verifier is installed, as in OAuth mode),
		 * captures the installed-app id, creates and deletes per-device event
		 * subscriptions automatically and dispatches device events. It never runs
		 * the OAuth flow, never stores tokens and never refreshes them: no
		 * client secret and no keychain are involved.
		 *
		 * Feed it tokens with setAccessToken() - at startup and after every
		 * refresh - and react to accessTokenRejected by refreshing (asynchronously)
		 * and calling setAccessToken() again. No background polling, as in OAuth
		 * mode (startPolling() remains available).
		 *
		 * Example:
		 * @code
		 * Client client(ExternalTokens{}, std::make_unique<MyGatewayServer>(),
		 *     std::make_unique<MyStateStore>());
		 * client.accessTokenRejected += [&](std::string) { myOAuth.refreshAsync(); };
		 * myOAuth.onToken([&](const Token& t) { client.setAccessToken(t.access, t.installedAppId); });
		 * @endcode
		 * @param httpServer Receives the webhook requests; Client binds and
		 *        starts it. Its OAuth route is never used and
		 *        fullOAuthCallbackUri() is never called. May be null if you
		 *        deliver webhook requests yourself through
		 *        handleWebhook(const WebhookRequest&).
		 * @param storageProvider Plain state storage (the installed-app id);
		 *        may be null to keep that state in memory only
		 */
		Client(ExternalTokens, std::unique_ptr<IHttpServer> httpServer,
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
		 *
		 * Thread-safe in every mode, callable from any thread any number of
		 * times, and never performs network I/O.
		 *
		 * - **PAT / OAuth mode** - replaces the token used by subsequent requests
		 *   (and records @p installedAppId when non-empty, writing it to the
		 *   plain storage). Nothing else happens.
		 * - **External-tokens mode** (see the ExternalTokens constructor) - this
		 *   is how the application signs the Client in and keeps it signed in,
		 *   and it never blocks: the token is adopted at once and
		 *   @p installedAppId (SmartThings' token response carries
		 *   @c installed_app_id) is recorded when non-empty; then, on the
		 *   Client's internal worker thread and never on the caller's, the id
		 *   is persisted, pending automatic device subscriptions are flushed
		 *   and, when this token signs the Client in (the first token, or the
		 *   first one after a sign-out), authenticationCompleted fires with
		 *   success. Call it again with every refreshed token.
		 *   An empty @p token signs the Client out: API calls then fail with
		 *   ErrorCode::AuthenticationRequired (getters return empty), and
		 *   nothing is deleted - the installed-app id, subscriptions and
		 *   Devices stay as they are until a new token arrives.
		 * @param token A SmartThings OAuth 2.0 Bearer token (PAT or OAuth access
		 *        token), or empty to sign out
		 * @param installedAppId The installed-app id this token belongs to, or
		 *        empty to keep the current one
		 */
		void setAccessToken(const std::string& token, const std::string& installedAppId = {});

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
		// In external-tokens mode the app supplies tokens instead:
		//   client.accessTokenRejected += [](std::string) { /* refresh, async */ };
		//   client.setAccessToken(token, installedAppId); // at start + each refresh
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
		 * In external-tokens mode: fires (always with success) when
		 * setAccessToken() signs the Client in - the first token, or the first
		 * one after a sign-out - but not for later refreshed tokens.
		 * @note Fires on the HTTP server's thread for the browser flow, and on
		 *       the Client's internal worker thread in external-tokens mode.
		 */
		ReactiveLitepp::Event<Result<void>> authenticationCompleted;

		/**
		 * @brief Fired when the API rejects (HTTP 401) the current external token
		 *
		 * External-tokens mode only (never fired in PAT or OAuth mode, where
		 * the library handles 401s itself). The argument is the rejected token,
		 * so a handler that has already moved on to a newer token can ignore a
		 * late report. Fired at most once per token: further 401s for the same
		 * token stay silent until setAccessToken() supplies a token again (even
		 * the same one). A report is dropped if the token was replaced before
		 * it could be delivered.
		 *
		 * The library never waits for a new token: the request that got the 401
		 * returns its error (ErrorCode::AuthenticationFailed, or an empty
		 * result for getters) right away, and pending automatic subscriptions
		 * are retried once setAccessToken() delivers the next token. Refresh
		 * asynchronously and call setAccessToken() with the result.
		 * @note Fires on the Client's internal worker thread, never inside the
		 *       call that received the 401, so the handler may call back into
		 *       the Client (including setAccessToken()). Keep it short: it
		 *       delays the worker's other jobs, and must not wait on the thread
		 *       destroying the Client.
		 */
		ReactiveLitepp::Event<std::string> accessTokenRejected;

		/**
		 * @brief Make this OAuth Client authenticated, as silently as possible
		 *
		 * Tries, in order: the stored access token (if not expired), the stored
		 * refresh token (rotating + persisting the new pair), and finally the
		 * interactive flow - firing openBrowserRequested and returning
		 * immediately. Call once at startup; tokens persisting across runs
		 * means the browser typically only opens the very first time.
		 *
		 * In external-tokens mode there is nothing to run: the Client is
		 * authenticated if and only if setAccessToken() supplied a token. This
		 * returns @c true or @c false accordingly, without any network call and
		 * without ever firing openBrowserRequested.
		 * @return On success, @c true if authenticated right now, @c false if
		 *         the browser step is pending (wait via waitForAuthentication()
		 *         or authenticationCompleted) - or, in external-tokens mode, if
		 *         no token has been supplied yet. An error Result if this isn't
		 *         an OAuth Client, the server failed to start, or a refresh
		 *         failed for a reason consent can't fix (e.g. network down).
		 */
		Result<bool> authenticate();

		/**
		 * @brief Block until the pending authentication finishes (or times out)
		 *
		 * In external-tokens mode this never blocks (a token can only come from
		 * the application itself, so waiting could deadlock it): it returns at
		 * once with success if a token is set, or ErrorCode::AuthenticationRequired
		 * if not. Observe authenticationCompleted to learn when a token arrives.
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
		 * @note This overload has no request headers to check, so it does NOT
		 *       verify the HTTP-Signature - it trusts its input. The
		 *       IHttpServer delivers real requests through the WebhookRequest
		 *       overload below, which does verify when a signature verifier is
		 *       configured (the default in OAuth mode).
		 */
		WebhookResponse handleWebhook(const std::string& rawBody);

		/**
		 * @brief Process one webhook request, verifying its signature first
		 *
		 * The full-fidelity entry point the embedded/injected IHttpServer calls:
		 * given the raw body plus the request line and headers, it first runs the
		 * configured IWebhookSignatureVerifier (rejecting the request with HTTP
		 * 401 if the HTTP-Signature is absent, malformed or invalid), then handles
		 * the messageType exactly as the string overload does. With no verifier
		 * set, verification is skipped.
		 * @param request The raw inbound request (body, request line, headers)
		 * @return The HTTP response to return, plus which messageType was handled
		 */
		WebhookResponse handleWebhook(const WebhookRequest& request);

		/**
		 * @brief Set (or clear) the webhook HTTP-Signature verifier
		 *
		 * OAuth-mode Clients install a default verifier automatically (see
		 * makeDefaultWebhookSignatureVerifier()), so inbound webhooks are
		 * cryptographically verified out of the box. Replace it to customize the
		 * behavior (e.g. a different crypto backend on non-Windows), or pass
		 * @c nullptr to turn verification off.
		 * @param verifier The verifier to use, or nullptr to disable verification
		 */
		void setWebhookSignatureVerifier(std::unique_ptr<IWebhookSignatureVerifier> verifier);

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
		// _externalTokens implies _isOAuth, with no authenticator and no keychain.
		bool _isOAuth = false;
		bool _externalTokens = false;
		oauth2::OAuth2Config _oauthConfig;
		std::unique_ptr<oauth2::OAuth2Authenticator> _authenticator;
		std::unique_ptr<IHttpServer> _httpServer;
		std::unique_ptr<IStorageProvider> _keychain; // secrets: the OAuth token pair
		std::unique_ptr<IStorageProvider> _storage;  // plain state: installed-app id
		// Verifies inbound webhooks' HTTP-Signature; null disables verification
		// (default on a PAT Client, installed automatically in OAuth mode).
		std::unique_ptr<IWebhookSignatureVerifier> _webhookVerifier;
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

		// External-tokens mode: the last token reported through
		// accessTokenRejected (cleared by setAccessToken()), so a token is
		// reported once, not once per failing request. Guarded by _authMutex.
		mutable std::string _rejectedToken;

		// External-tokens mode: a worker thread runs everything setAccessToken()
		// and the 401 path must not do on the caller's thread - storage writes,
		// subscription flushes (network) and consumer event handlers - so that
		// neither ever blocks. Jobs are coalesced flags; all guarded by
		// _workerMutex (_workerStop is atomic so long loops can poll it).
		std::thread _worker;
		mutable std::mutex _workerMutex;
		mutable std::condition_variable _workerCv;
		std::atomic<bool> _workerStop{ false };
		bool _workerFlush = false;                      // flush pending subscriptions
		bool _workerAnnounce = false;                   // fire authenticationCompleted(success)
		std::optional<std::string> _workerPersistAppId; // installed-app id to write to _storage
		mutable std::vector<std::string> _workerRejected; // tokens to report via accessTokenRejected

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
		/// In external-tokens mode a 401 never waits for anything: the request
		/// is retried once only if setAccessToken() already supplied a newer
		/// token meanwhile, and a still-rejected token is reported through
		/// accessTokenRejected (see reportRejectedToken()).
		HttpResponse getWithAuth(const std::string& url) const;

		/// POST equivalent of getWithAuth().
		HttpResponse postWithAuth(const std::string& url, const std::string& body) const;

		/// The shared 401 policy behind getWithAuth()/postWithAuth(): @p send
		/// performs the request with the given auth headers (possibly twice).
		HttpResponse sendWithAuth(
			const std::function<HttpResponse(const std::map<std::string, std::string>& headers)>& send) const;

		/// _access_token read under _authMutex.
		std::string currentAccessToken() const;

		/// External-tokens mode: queue an accessTokenRejected report for
		/// @p token if it is still the current token and not reported yet.
		/// Non-blocking; a no-op in the other modes and for an empty token.
		void reportRejectedToken(const std::string& token) const;

		/// External-tokens mode worker: runs the queued jobs (see _worker).
		void workerLoop();

		/// Stop and join the worker (no-op if it never started).
		void stopWorker();

		/// Under _registryMutex: locks every weak_ptr in the registry, pruning
		/// (and dropping empty buckets for) any device id with no survivors, and
		/// returns {deviceId, live devices} pairs for pollNow() to act on outside
		/// the lock.
		std::vector<std::pair<std::string, std::vector<std::shared_ptr<Device>>>> snapshotRegistry();

		/// Under _registryMutex: lock (and prune) the registry bucket for one
		/// device id, returning its currently-live Device objects. Used to fan a
		/// single webhook device event out to every Device wrapping that id.
		std::vector<std::shared_ptr<Device>> liveDevicesFor(const std::string& deviceId);

		/// Handle a webhook request's messageType (PING/EVENT/...) without any
		/// signature verification - the shared core both handleWebhook() overloads
		/// call once the request is (or is trusted to be) authentic.
		WebhookResponse processWebhook(const WebhookRequest& request);

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
