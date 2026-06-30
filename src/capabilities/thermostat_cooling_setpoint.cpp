#include "smartthings4cpp/capabilities/thermostat_cooling_setpoint.h"

#include <utility>

namespace smartthings4cpp {

	ThermostatCoolingSetpoint::ThermostatCoolingSetpoint(int version, std::string componentId,
		std::string deviceId, Client* client)
		: Capability("thermostatCoolingSetpoint", version, std::move(componentId), std::move(deviceId), client) {
	}

	void ThermostatCoolingSetpoint::parseStatus(const nlohmann::json&) {
		double setpoint = attributeNumber("coolingSetpoint", _setpoint);
		SetPropertyValueAndNotify<&ThermostatCoolingSetpoint::CoolingSetpoint>(_setpoint, setpoint);

		std::string unit = attributeUnit("coolingSetpoint", _unit);
		SetPropertyValueAndNotify<&ThermostatCoolingSetpoint::Unit>(_unit, unit);
	}

	Result<void> ThermostatCoolingSetpoint::setCoolingSetpoint(double setpoint) {
		nlohmann::json args = nlohmann::json::array();
		args.push_back(setpoint);

		auto result = executeCommand("setCoolingSetpoint", args);
		if (result.isSuccess()) {
			setLocalAttribute("coolingSetpoint", setpoint);
		}
		return result;
	}

} // namespace smartthings4cpp
