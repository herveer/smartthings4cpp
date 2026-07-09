#pragma once

#include "types.h"
#include "webhook.h"
#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

/**
 * @file webhook_signature.h
 * @brief Cryptographic verification of inbound SmartThings webhook requests
 *
 * Every request SmartThings POSTs to a webhook is signed with an HTTP Signature
 * (IETF "Signing HTTP Messages" draft-cavage-03): an @c Authorization header of
 * the form
 * @code
 *   Authorization: Signature keyId="/pl/useast1/<uuid>",
 *       headers="(request-target) digest date", algorithm="rsa-sha256",
 *       signature="<base64 RSA-SHA256 signature>"
 * @endcode
 * The signature is computed over a canonical string assembled from the listed
 * headers; the body's integrity rides along through the @c Digest header
 * (@c SHA-256=<base64>), which is one of the signed headers. Verifying it proves
 * the request genuinely came from SmartThings and was not tampered with - the
 * one real security gap left in the webhook path, since the endpoint otherwise
 * trusts whatever POSTs to it.
 *
 * Verification needs SmartThings' RSA public key, which is fetched at
 * @c <keyServerBaseUrl><keyId> (e.g. https://key.smartthings.com/pl/...). These
 * keys rotate, so the key id travels in every signature and the default verifier
 * caches fetched keys only briefly (see WebhookSignatureConfig::keyCacheTtl).
 *
 * The parsing/canonicalization helpers below are pure and platform-independent
 * (and unit-tested offline). The RSA verification itself is done natively with
 * Windows CNG (BCrypt/CryptoAPI) - there is no OpenSSL in this toolchain - so the
 * default verifier only functions on Windows; on other platforms inject your own
 * IWebhookSignatureVerifier through Client::setWebhookSignatureVerifier().
 */

namespace smartthings4cpp {

	/// Pure, offline HTTP-Signature parsing/canonicalization helpers. Separated
	/// from the crypto so the fiddly string handling can be unit-tested without a
	/// key, a network, or a platform crypto backend.
	namespace http_signature {

		/**
		 * @brief The fields parsed out of a "Signature ..." authorization header
		 */
		struct SignatureParams {
			std::string keyId;             ///< Path used to retrieve the public key
			std::string algorithm;         ///< e.g. "rsa-sha256" (may be empty if the header omitted it)
			std::string signatureBase64;   ///< The base64-encoded signature bytes
			std::vector<std::string> headers; ///< Ordered, lowercased names of the signed headers
		};

		/**
		 * @brief Parse the value of an @c Authorization (scheme @c Signature) or
		 *        @c Signature header
		 *
		 * Accepts the value with or without a leading @c "Signature " scheme
		 * token and tolerates spacing around the comma-separated parameters.
		 * @param headerValue The raw header value
		 * @return The parsed parameters, or std::nullopt if it is not a
		 *         well-formed Signature header (no keyId or no signature)
		 */
		std::optional<SignatureParams> parseSignatureParams(const std::string& headerValue);

		/**
		 * @brief Build the canonical signing string for a set of signed headers
		 *
		 * For each name in @p signedHeaders (in order) emits one @c "name: value"
		 * line joined by @c '\n'. The synthetic @c (request-target) becomes
		 * @c "<method-lowercase> <path>"; every other name is looked up on
		 * @p request (case-insensitively) with surrounding whitespace trimmed.
		 * @param signedHeaders The ordered header names from SignatureParams
		 * @param request The request whose headers/line supply the values
		 * @return The signing string, or std::nullopt if a listed header is
		 *         missing from @p request (the signature cannot be reconstructed)
		 */
		std::optional<std::string> buildSigningString(
			const std::vector<std::string>& signedHeaders, const WebhookRequest& request);

		/**
		 * @brief Parse an HTTP-date header (RFC 7231 IMF-fixdate) to a time point
		 *
		 * Handles the @c "Sun, 06 Nov 1994 08:49:37 GMT" form SmartThings sends.
		 * Locale-independent (month names are matched from a fixed table).
		 * @param httpDate The @c Date header value
		 * @return The instant it denotes, or std::nullopt if unparseable
		 */
		std::optional<std::chrono::system_clock::time_point> parseHttpDate(const std::string& httpDate);

	} // namespace http_signature

	/**
	 * @brief Verifies the HTTP-Signature on an inbound webhook request
	 *
	 * Client owns one of these (created by default in OAuth mode) and runs it on
	 * every webhook it receives before acting on the request. Implement it
	 * yourself and inject it via Client::setWebhookSignatureVerifier() to use a
	 * different crypto backend (e.g. OpenSSL on non-Windows), the SmartApp
	 * public-key scheme, or a permissive verifier in tests.
	 */
	class IWebhookSignatureVerifier {
	public:
		virtual ~IWebhookSignatureVerifier() = default;

		/**
		 * @brief Verify a request's signature
		 * @param request The raw request (body, request line and headers)
		 * @return Success if the signature is present, well-formed and valid;
		 *         an @c AuthenticationFailed Result describing why otherwise
		 */
		virtual Result<void> verify(const WebhookRequest& request) = 0;
	};

	/// Given a signature's keyId, returns the PEM public key to verify with, or
	/// std::nullopt if it cannot be resolved. The default verifier uses this as
	/// its key source; passing your own to the factory overrides where keys come
	/// from (e.g. a local test fixture, or the SmartApp public-key scheme).
	using PublicKeyResolver = std::function<std::optional<std::string>(const std::string& keyId)>;

	/**
	 * @brief Tunables for the default webhook signature verifier
	 */
	struct WebhookSignatureConfig {
		/// Base URL the public key is fetched from: the request goes to
		/// @c keyServerBaseUrl + keyId. Overridable for testing.
		std::string keyServerBaseUrl = "https://key.smartthings.com";

		/// Optional replay protection: if non-zero, reject a request whose signed
		/// @c Date is more than this far from now. Off by default (0), matching
		/// the reference SmartThings SDK - the RSA signature already covers (and
		/// so authenticates) the Date, and an always-on window would falsely
		/// reject legitimate events on clock drift or delivery delay. Set e.g. to
		/// std::chrono::minutes(5) to opt in.
		std::chrono::seconds maxClockSkew{ 0 };

		/// How long a fetched public key is cached before being re-fetched. Keys
		/// rotate, so this is intentionally short.
		std::chrono::seconds keyCacheTtl{ std::chrono::hours(1) };
	};

	/**
	 * @brief Create the default HTTP-Signature verifier
	 *
	 * Fetches SmartThings' RSA public key from the key server (caching it per
	 * WebhookSignatureConfig) and verifies rsa-sha256 signatures natively via
	 * Windows CNG. On non-Windows platforms the crypto backend is unavailable and
	 * verify() fails with a clear message - inject your own implementation there.
	 * @param config Key server, clock-skew and cache settings
	 * @param keyResolver Optional override for the public-key source; when null
	 *        keys are fetched over HTTPS from @c config.keyServerBaseUrl
	 * @return A verifier instance
	 */
	std::unique_ptr<IWebhookSignatureVerifier> makeDefaultWebhookSignatureVerifier(
		WebhookSignatureConfig config = {}, PublicKeyResolver keyResolver = nullptr);

} // namespace smartthings4cpp
