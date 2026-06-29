#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>

/**
 * @file types.h
 * @brief Common types and definitions used throughout smartthings4cpp
 */

namespace smartthings4cpp {

	/**
	 * @brief A capability exposed by a device component
	 *
	 * In the SmartThings model a capability (e.g. "switch", "switchLevel",
	 * "colorControl") describes a set of commands and attributes a device
	 * supports. The device-description endpoint reports only the capability
	 * @c id and @c version; the concrete attribute values are fetched separately
	 * from the device status endpoint (planned for the device-control iteration).
	 */
	struct Capability {
		std::string id;       ///< Capability identifier (e.g. "switch")
		int version = 1;      ///< Capability version
	};

	/**
	 * @brief A category hint reported for a device component
	 */
	struct Category {
		std::string name;          ///< Category name (e.g. "Light", "Switch")
		std::string categoryType;  ///< Origin of the category (e.g. "manufacturer", "user")
	};

	/**
	 * @brief A component of a device
	 *
	 * Every device has at least a "main" component. Each component groups a set
	 * of capabilities and optional category hints.
	 */
	struct Component {
		std::string id;                          ///< Component id (e.g. "main")
		std::string label;                       ///< Human readable component label
		std::vector<Capability> capabilities;    ///< Capabilities exposed by this component
		std::vector<Category> categories;        ///< Category hints for this component
	};

	/**
	 * @brief A SmartThings location (a physical place, e.g. "Home")
	 */
	struct Location {
		std::string locationId;   ///< Unique location identifier
		std::string name;         ///< User-friendly location name
		std::string countryCode;  ///< ISO country code of the location
	};

	/**
	 * @brief A room within a location
	 */
	struct Room {
		std::string roomId;       ///< Unique room identifier
		std::string locationId;   ///< Identifier of the owning location
		std::string name;         ///< User-friendly room name
	};

	/**
	 * @brief Error codes for smartthings4cpp operations
	 */
	enum class ErrorCode {
		Success = 0,
		NetworkError,
		AuthenticationRequired,
		AuthenticationFailed,
		InvalidRequest,
		ResourceNotFound,
		InvalidParameter,
		RateLimited,
		ApiError,
		TimeoutError,
		UnknownError
	};

	/**
	 * @brief Result type for operations that can fail
	 * @tparam T The type of the result value
	 */
	template<typename T>
	struct Result {
		std::optional<T> value;
		ErrorCode error;
		std::string error_message;

		Result() : error(ErrorCode::Success) {}
		explicit Result(T val) : value(std::move(val)), error(ErrorCode::Success) {}
		Result(ErrorCode err, const std::string& msg = "")
			: error(err), error_message(msg) {
		}

		bool isSuccess() const { return error == ErrorCode::Success; }
		bool hasValue() const { return value.has_value(); }

		operator bool() const { return isSuccess(); }
	};

	/**
	 * @brief Specialization for void Result
	 */
	template<>
	struct Result<void> {
		ErrorCode error;
		std::string error_message;

		Result() : error(ErrorCode::Success) {}
		Result(ErrorCode err, const std::string& msg = "")
			: error(err), error_message(msg) {
		}

		bool isSuccess() const { return error == ErrorCode::Success; }

		operator bool() const { return isSuccess(); }
	};

} // namespace smartthings4cpp
