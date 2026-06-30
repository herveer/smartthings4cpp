#pragma once

#include "../capability.h"

/**
 * @file contact_sensor.h
 * @brief "contactSensor" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c contactSensor capability: open/closed contact state
	 *
	 * Attribute: @c contact ("open"/"closed"). No commands.
	 */
	class ContactSensor : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "contactSensor";

		ContactSensor(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief Raw contact state, "open" or "closed" (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> Contact{
			[this]() { return _contact; }
		};

		/** @brief Whether the contact is open (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<bool> IsOpen{
			[this]() { return _contact == "open"; }
		};

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _contact = "closed";
	};

} // namespace smartthings4cpp
