#pragma once

#include "types.h"
#include <string>
#include <nlohmann/json_fwd.hpp>

/**
 * @file webhook.h
 * @brief Value types for SmartThings OAuth event subscriptions and the webhook
 *        messageType
 *
 * SmartThings only delivers real push notifications to an OAuth "API Access"
 * app, and only over a webhook: you create subscriptions with the REST API
 * (see Client::subscribeTo()) and SmartThings POSTs device events to the Target
 * URL registered on your app. This header holds the plain value types shared by
 * that flow; the behaviour lives on Client (client.h) and in src/webhook.cpp.
 *
 * As with OAuth2 (see oauth2/), the library does the *protocol*, not the I/O:
 * it never hosts the HTTPS server SmartThings calls. Your application receives
 * the POST (behind a public URL / tunnel such as ngrok), hands the raw body to
 * Client::handleWebhook(), and writes back the WebhookResponse it returns.
 */

namespace smartthings4cpp {

	/**
	 * @brief A request to subscribe to a device (or capability) event stream
	 *
	 * Mirrors the SmartThings @c SubscriptionRequest (sourceType DEVICE): a
	 * subscription narrows to a device, optionally to a single capability and
	 * attribute. Use the @c "*" wildcard to match every capability/attribute on
	 * the device.
	 */
	struct SubscriptionRequest {
		std::string deviceId;                ///< Device to subscribe to (required)
		bool stateChangeOnly = true;         ///< Only deliver events where the value actually changed
		std::string subscriptionName;        ///< Optional name; auto-generated if left empty
	};

	/**
	 * @brief A subscription as returned by the SmartThings API
	 */
	struct Subscription {
		std::string id;              ///< Server-assigned subscription id (used to delete it)
		std::string installedAppId;  ///< Installed-app the subscription belongs to
		std::string sourceType;      ///< e.g. "DEVICE"
		std::string deviceId;        ///< Echoed device id (for DEVICE subscriptions)
		std::string capability;      ///< Echoed capability
		std::string attribute;       ///< Echoed attribute
		std::string componentId;     ///< Echoed component id
	};

	/**
	 * @brief The webhook messageType phase Client::handleWebhook() just processed
	 *
	 * Surfaced on WebhookResponse so a caller can react (e.g. create
	 * subscriptions after Install) without re-parsing the request body.
	 */
	enum class WebhookMessageType {
		Unknown,       ///< Body was missing/unparseable or an unrecognized messageType
		Ping,          ///< Health check (challenge/response)
		Confirmation,  ///< Target-URL verification handshake
		Configuration, ///< Configuration page request
		Install,       ///< App installed (installedAppId + auth token delivered)
		Update,        ///< App configuration updated
		Event,         ///< One or more device (or platform) events
		Uninstall,     ///< App uninstalled
		OAuthCallback  ///< OAuth callback phase
	};

	/**
	 * @brief The HTTP response your webhook endpoint must return to SmartThings
	 *
	 * Client::handleWebhook() computes this from the incoming messageType request;
	 * your server writes @c body with status @c statusCode (and
	 * @c Content-Type: application/json). @c messageType reports what was handled.
	 */
	struct WebhookResponse {
		int statusCode = 200;                            ///< HTTP status to return
		std::string body = "{}";                         ///< JSON body to return
		WebhookMessageType messageType = WebhookMessageType::Unknown; ///< Phase that was handled
	};

	/**
	 * @brief Build the JSON body for POST /installedapps/{id}/subscriptions
	 *
	 * Produces the @c {"sourceType":"DEVICE","device":{...}} shape SmartThings
	 * expects. Exposed (and pure) so it can be unit-tested without a network,
	 * mirroring buildCommandsBody().
	 * @param request The subscription to create; @c subscriptionName is filled in
	 *        (from deviceId/capability) when empty
	 * @return The request body
	 */
	nlohmann::json buildSubscriptionBody(const SubscriptionRequest& request);

} // namespace smartthings4cpp
