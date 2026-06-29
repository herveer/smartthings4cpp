#pragma once

#include "types.h"
#include <stdexcept>
#include <string>

/**
 * @file exceptions.h
 * @brief Exception classes for smartthings4cpp
 */

namespace smartthings4cpp {

/**
 * @brief Base exception class for all smartthings4cpp exceptions
 */
class SmartThingsException : public std::runtime_error {
public:
    explicit SmartThingsException(const std::string& message)
        : std::runtime_error(message), error_code_(ErrorCode::UnknownError) {}

    SmartThingsException(ErrorCode code, const std::string& message)
        : std::runtime_error(message), error_code_(code) {}

    /**
     * @brief Get the error code associated with this exception
     * @return ErrorCode value
     */
    ErrorCode getErrorCode() const noexcept { return error_code_; }

private:
    ErrorCode error_code_;
};

/**
 * @brief Exception thrown when network operations fail
 */
class NetworkException : public SmartThingsException {
public:
    explicit NetworkException(const std::string& message)
        : SmartThingsException(ErrorCode::NetworkError, message) {}
};

/**
 * @brief Exception thrown when authentication fails
 */
class AuthenticationException : public SmartThingsException {
public:
    explicit AuthenticationException(const std::string& message)
        : SmartThingsException(ErrorCode::AuthenticationFailed, message) {}
};

/**
 * @brief Exception thrown when a requested resource is not found
 */
class ResourceNotFoundException : public SmartThingsException {
public:
    explicit ResourceNotFoundException(const std::string& message)
        : SmartThingsException(ErrorCode::ResourceNotFound, message) {}
};

/**
 * @brief Exception thrown when invalid parameters are provided
 */
class InvalidParameterException : public SmartThingsException {
public:
    explicit InvalidParameterException(const std::string& message)
        : SmartThingsException(ErrorCode::InvalidParameter, message) {}
};

/**
 * @brief Exception thrown when the SmartThings API returns an error response
 */
class ApiException : public SmartThingsException {
public:
    explicit ApiException(const std::string& message)
        : SmartThingsException(ErrorCode::ApiError, message) {}
};

/**
 * @brief Exception thrown when the API rate limit is exceeded (HTTP 429)
 */
class RateLimitException : public SmartThingsException {
public:
    explicit RateLimitException(const std::string& message)
        : SmartThingsException(ErrorCode::RateLimited, message) {}
};

/**
 * @brief Exception thrown when an operation times out
 */
class TimeoutException : public SmartThingsException {
public:
    explicit TimeoutException(const std::string& message)
        : SmartThingsException(ErrorCode::TimeoutError, message) {}
};

/**
 * @brief Convert ErrorCode to a human-readable string
 * @param code The error code to convert
 * @return String representation of the error code
 */
inline std::string errorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::Success:
            return "Success";
        case ErrorCode::NetworkError:
            return "Network Error";
        case ErrorCode::AuthenticationRequired:
            return "Authentication Required";
        case ErrorCode::AuthenticationFailed:
            return "Authentication Failed";
        case ErrorCode::InvalidRequest:
            return "Invalid Request";
        case ErrorCode::ResourceNotFound:
            return "Resource Not Found";
        case ErrorCode::InvalidParameter:
            return "Invalid Parameter";
        case ErrorCode::RateLimited:
            return "Rate Limited";
        case ErrorCode::ApiError:
            return "API Error";
        case ErrorCode::TimeoutError:
            return "Timeout Error";
        case ErrorCode::UnknownError:
            return "Unknown Error";
        default:
            return "Unknown";
    }
}

} // namespace smartthings4cpp
