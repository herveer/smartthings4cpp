#pragma once

#include <chrono>
#include <string>
#include <vector>

/**
 * @file oauth2_types.h
 * @brief Configuration and token types for the SmartThings OAuth 2.0 authorization-code flow
 */

namespace smartthings4cpp {
namespace oauth2 {

    /**
     * @brief Configuration for a self-registered SmartThings "OAuth-In App"
     *
     * Each developer registers their own app (`smartthings apps:create`) and
     * gets back a @c clientId/@c clientSecret pair, the same self-service model
     * as generating a Personal Access Token. smartthings4cpp does not embed or
     * share a client secret of its own.
     */
    struct OAuth2Config {
        std::string clientId;
        std::string clientSecret;
        std::string appUri;
        std::vector<std::string> scopes = { "r:devices:*", "x:devices:*", "r:locations:*" };
        std::string authorizeUrl = "https://api.smartthings.com/oauth/authorize";
        std::string tokenUrl = "https://api.smartthings.com/oauth/token";
    };

    /**
     * @brief An OAuth 2.0 access/refresh token pair returned by SmartThings
     */
    struct OAuth2Token {
        std::string accessToken;
        std::string refreshToken;
        std::string tokenType;   ///< Normally "bearer"
        std::string scope;
        /// Identifier of the app installation this token belongs to, when the
        /// token endpoint provides it (SmartThings includes installed_app_id
        /// for API Access apps). This is what the Subscriptions API is keyed on.
        std::string installedAppId;
        std::chrono::system_clock::time_point issuedAt;
        std::chrono::seconds expiresIn{ 0 };

        /**
         * @brief Whether this token is expired (or expiring within @p skew)
         * @param skew Safety margin: treat the token as expired this long before
         *             its actual expiry, so callers can refresh proactively
         *             instead of racing an in-flight request against expiry.
         */
        bool isExpired(std::chrono::seconds skew = std::chrono::seconds{ 60 }) const {
            return std::chrono::system_clock::now() >= (issuedAt + expiresIn - skew);
        }
    };

} // namespace oauth2
} // namespace smartthings4cpp
