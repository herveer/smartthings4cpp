#pragma once

#include "../capability.h"

/**
 * @file thermostat_cooling_setpoint.h
 * @brief "thermostatCoolingSetpoint" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c thermostatCoolingSetpoint capability: target cooling temperature
	 *
	 * Attribute: @c coolingSetpoint ({value: number, unit: "C"|"F"}). Command:
	 * @c setCoolingSetpoint(setpoint).
	 */
	class ThermostatCoolingSetpoint : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "thermostatCoolingSetpoint";

		ThermostatCoolingSetpoint(int version, std::string componentId, std::string deviceId, Client* client);

		/**
		 * @brief Get or set the cooling setpoint value
		 *
		 * Assigning issues @c setCoolingSetpoint with the new value.
		 */
		ReactiveLitepp::Property<double> CoolingSetpoint{
			[this]() { return _setpoint; },
			[this](double& value) {
				NotifyPropertyChanging<&ThermostatCoolingSetpoint::CoolingSetpoint>();
				setCoolingSetpoint(value);
			}
		};

		/** @brief Setpoint unit, "C" or "F" (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> Unit{
			[this]() { return _unit; }
		};

		/** @brief Set the cooling setpoint (@c setCoolingSetpoint command) */
		Result<void> setCoolingSetpoint(double setpoint);

		std::vector<std::string> commandNames() const override { return { "setCoolingSetpoint" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		double _setpoint = 0.0;
		std::string _unit;
	};

} // namespace smartthings4cpp
