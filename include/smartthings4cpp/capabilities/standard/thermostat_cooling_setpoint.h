#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c thermostatCoolingSetpoint capability (generated). */
	class ThermostatCoolingSetpoint : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "thermostatCoolingSetpoint";

		ThermostatCoolingSetpoint(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> CoolingSetpointRange = MakeReadonlyProperty<&ThermostatCoolingSetpoint::CoolingSetpointRange>([this]() { return _coolingSetpointRange; });
		ReactiveLitepp::ReadonlyProperty<double> CoolingSetpoint = MakeReadonlyProperty<&ThermostatCoolingSetpoint::CoolingSetpoint>([this]() { return _coolingSetpoint; });

		Result<void> setCoolingSetpoint(double setpoint);
		std::vector<std::string> commandNames() const override { return { "setCoolingSetpoint" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _coolingSetpointRange;
		double _coolingSetpoint = 0.0;
	};

} // namespace standard
} // namespace smartthings4cpp
