#pragma once

#include <string>

/**
 * @file base64.h
 * @brief Minimal base64 encoding utility
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

} // namespace base64

} // namespace smartthings4cpp
