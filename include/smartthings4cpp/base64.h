#pragma once

#include <optional>
#include <string>

/**
 * @file base64.h
 * @brief Minimal base64 encoding/decoding utility
 */

namespace smartthings4cpp {

/**
 * @brief Utility functions for base64 encoding
 */
namespace base64 {

    /**
     * @brief Base64-encode a byte string
     * @param input The bytes to encode
     * @return The base64-encoded (RFC 4648, with '=' padding) representation of @p input
     */
    std::string encode(const std::string& input);

    /**
     * @brief Base64-decode a string back to its raw bytes
     *
     * Accepts standard RFC 4648 base64 (alphabet @c A-Za-z0-9+/ with optional
     * '=' padding). ASCII whitespace (spaces / line breaks) is ignored so a
     * wrapped/multi-line encoding still decodes. Used to recover the binary
     * HTTP-Signature from a webhook's Authorization header, so it is strict:
     * any other character makes the input invalid.
     * @param input The base64 text to decode
     * @return The decoded bytes, or std::nullopt if @p input is not valid base64
     */
    std::optional<std::string> decode(const std::string& input);

} // namespace base64

} // namespace smartthings4cpp
