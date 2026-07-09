#pragma once

#include "types.h"
#include "webhook.h"
#include <functional>
#include <memory>
#include <mutex>
#include <string>

/**
 * @file http_server.h
 * @brief Inbound HTTP server abstraction for the OAuth-mode Client
 *
 * An OAuth Client needs two inbound routes: the OAuth redirect (the browser
 * lands there with the authorization code) and the webhook (SmartThings POSTs
 * messageType requests and device events there). By default Client embeds a small
 * local HTTP server (see makeDefaultHttpServer()) so both are handled without
 * any consumer wiring; apps that already run their own HTTP stack implement
 * IHttpServer instead and inject it through Client's DI constructor.
 *
 * The default server listens on 127.0.0.1 in plain HTTP. SmartThings requires
 * the *public* Target URL to be HTTPS with a valid certificate - terminate TLS
 * at a tunnel/reverse proxy (e.g. `ngrok http <port>`) that forwards to the
 * local port. The OAuth redirect can go straight to http://localhost:<port>.
 */

namespace smartthings4cpp {

	/**
	 * @brief Abstraction over the two inbound routes an OAuth Client needs
	 *
	 * Concrete implementations receive requests however they like (own socket,
	 * existing web framework, ...) and forward them via the protected
	 * onOAuthReceived()/onWebhookReceived() helpers. The Client wires itself in
	 * with bind() before calling start(); implementations never need to know
	 * anything about OAuth or the webhook messageType.
	 */
	class IHttpServer {
	public:
		/// Receives whatever the OAuth redirect carried - the full request
		/// target, e.g. "/oauth/callback?code=...&state=...".
		using OAuthCallback = std::function<void(std::string args)>;
		/// Receives a raw webhook request (body, request line and headers - see
		/// WebhookRequest) and returns the WebhookResponse to send back (its
		/// statusCode and JSON body). Passing the raw request rather than a parsed
		/// body is what lets the callback verify the HTTP-Signature, which is
		/// computed over the exact bytes and the request line.
		using WebhookCallback = std::function<WebhookResponse(const WebhookRequest& request)>;

		virtual ~IHttpServer() = default;

		/**
		 * @brief Wire the callbacks requests are forwarded to
		 *
		 * Called by Client during construction, before start(). Separate from
		 * the constructor so a consumer can build a custom IHttpServer first
		 * and hand it to Client, which binds itself afterwards.
		 */
		void bind(OAuthCallback oauthCallback, WebhookCallback webhookCallback) {
			std::lock_guard<std::mutex> lock(_callbackMutex);
			_oauthCallback = std::move(oauthCallback);
			_webhookCallback = std::move(webhookCallback);
		}

		/**
		 * @brief Start accepting requests
		 * @return Success once the server is ready, or why it could not start
		 *         (e.g. the port is already in use)
		 */
		virtual Result<void> start() = 0;

		/// Stop accepting requests. Safe to call when not started.
		virtual void stop() = 0;

		/**
		 * @brief The full redirect URI the OAuth route is reachable at from 
		 * SmartThings (the public Target URL, not the local listen address).
		 *
		 * Client passes this to the authorize URL as redirect_uri, so it must
		 * exactly match a redirect URI registered on the OAuth-In App
		 * (e.g. "http://123.456.789.012:9753/oauth/callback").
		 */
		virtual std::string fullOAuthCallbackUri() = 0;

	protected:
		/// Implementations call this when a request hits the OAuth route.
		void onOAuthReceived(std::string args) {
			OAuthCallback callback;
			{
				std::lock_guard<std::mutex> lock(_callbackMutex);
				callback = _oauthCallback;
			}
			if (callback) {
				callback(std::move(args));
			}
		}

		/// Implementations call this when a request hits the webhook route.
		/// @return The response to send back (a plain 200 "{}" if unbound).
		WebhookResponse onWebhookReceived(const WebhookRequest& request) {
			WebhookCallback callback;
			{
				std::lock_guard<std::mutex> lock(_callbackMutex);
				callback = _webhookCallback;
			}
			if (callback) {
				return callback(request);
			}
			return WebhookResponse{};
		}

	private:
		std::mutex _callbackMutex;
		OAuthCallback _oauthCallback;
		WebhookCallback _webhookCallback;
	};

	/**
	 * @brief Create the default embedded HTTP server (cpp-httplib based)
	 *
	 * Plain HTTP on 127.0.0.1, one background thread; GETs on
	 * @p oauthCallbackRoute go to the OAuth callback (the browser gets a small
	 * "you can close this tab" page), POSTs on @p webhookCallbackRoute are
	 * parsed as JSON (400 on invalid JSON) and go to the webhook callback.
	 * @param port Port to listen on; 0 picks an ephemeral free port (the actual
	 *        one is reflected in fullOAuthCallbackUri() after start())
	 * @param oauthCallbackRoute Path of the OAuth redirect route (leading '/'
	 *        added if missing), e.g. "/oauth/callback"
	 * @param webhookCallbackRoute Path of the webhook route, e.g. "/webhook"
	 */
	std::unique_ptr<IHttpServer> makeDefaultHttpServer(int port,
		std::string oauthCallbackRoute, std::string webhookCallbackRoute, std::string externalUri);

} // namespace smartthings4cpp
