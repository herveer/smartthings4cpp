#pragma once

/**
 * @file smartthings4cpp.h
 * @brief Main header file for the smartthings4cpp library
 *
 * Include this header to access all smartthings4cpp functionality.
 */

#include "smartthings4cpp/types.h"
#include "smartthings4cpp/exceptions.h"
#include "smartthings4cpp/http_client.h"
#include "smartthings4cpp/json_utils.h"
#include "smartthings4cpp/base64.h"
#include "smartthings4cpp/capability.h"
#include "smartthings4cpp/capabilities.h"
#include "smartthings4cpp/component.h"
#include "smartthings4cpp/client.h"
#include "smartthings4cpp/device.h"
#include "smartthings4cpp/oauth2/oauth2_types.h"
#include "smartthings4cpp/oauth2/oauth2_authenticator.h"

/**
 * @namespace smartthings4cpp
 * @brief Main namespace for the smartthings4cpp library
 */
namespace smartthings4cpp {

/**
 * @brief Library version information
 */
struct Version {
    static constexpr int MAJOR = 0;
    static constexpr int MINOR = 1;
    static constexpr int PATCH = 0;

    static constexpr const char* STRING = "0.1.0";
};

} // namespace smartthings4cpp
