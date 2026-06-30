#pragma once

#include "../capability.h"

/**
 * @file refresh.h
 * @brief "refresh" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c refresh capability: ask the device to refresh its own state
	 *
	 * No attributes. Command: @c refresh.
	 *
	 * @note This issues the SmartThings @c refresh command to the device. To
	 *       re-fetch a capability's cached status from the cloud instead, use the
	 *       base Capability::refreshStatus().
	 */
	class Refresh : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "refresh";

		Refresh(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief Issue the device @c refresh command */
		Result<void> refresh();

		std::vector<std::string> commandNames() const override { return { "refresh" }; }
	};

} // namespace smartthings4cpp
