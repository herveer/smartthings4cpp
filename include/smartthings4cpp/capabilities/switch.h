#pragma once

#include "../capability.h"

/**
 * @file switch.h
 * @brief "switch" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c switch capability: turn a device on or off
	 *
	 * Attribute: @c switch ("on"/"off"). Commands: @c on, @c off.
	 */
	class Switch : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "switch";

		Switch(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief Raw switch state, "on" or "off" (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> State{
			[this]() { return _state; }
		};

		/** @brief Whether the switch is on (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<bool> IsOn{
			[this]() { return _state == "on"; }
		};

		/**
		 * @brief Get or set the on/off state
		 *
		 * Assigning issues the @c on or @c off command and optimistically updates
		 * the cached state.
		 */
		ReactiveLitepp::Property<bool> On{
			[this]() { return _state == "on"; },
			[this](bool& value) {
				NotifyPropertyChanging<&Switch::On>();
				value ? on() : off();
			}
		};

		/** @brief Turn the switch on (@c on command) */
		Result<void> on();
		/** @brief Turn the switch off (@c off command) */
		Result<void> off();
		/** @brief Toggle the switch based on its last known state */
		Result<void> toggle();

		std::vector<std::string> commandNames() const override { return { "on", "off" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _state = "off";
	};

} // namespace smartthings4cpp
