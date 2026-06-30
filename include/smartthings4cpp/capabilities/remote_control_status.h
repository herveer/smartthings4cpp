#pragma once

#include "../capability.h"
#include <string>

/**
 * @file remote_control_status.h
 * @brief "remoteControlStatus" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c remoteControlStatus capability: whether remote control is enabled
	 *
	 * Attribute: @c remoteControlEnabled ("true"/"false"). No commands.
	 */
	class RemoteControlStatus : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "remoteControlStatus";

		RemoteControlStatus(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief Raw remoteControlEnabled value, "true" or "false" (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> RemoteControlEnabled{
			[this]() { return _enabled; }
		};

		/** @brief Whether remote control is enabled (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<bool> Enabled{
			[this]() { return _enabled == "true"; }
		};

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _enabled = "false";
	};

} // namespace smartthings4cpp
