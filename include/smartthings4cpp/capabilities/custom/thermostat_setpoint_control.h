#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.thermostatSetpointControl capability (Samsung-proprietary; generated). */
	class ThermostatSetpointControl : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.thermostatSetpointControl";

		ThermostatSetpointControl(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<double> MinimumSetpoint = MakeReadonlyProperty<&ThermostatSetpointControl::MinimumSetpoint>([this]() { return _minimumSetpoint; });
		ReactiveLitepp::ReadonlyProperty<double> MaximumSetpoint = MakeReadonlyProperty<&ThermostatSetpointControl::MaximumSetpoint>([this]() { return _maximumSetpoint; });

		Result<void> raiseSetpoint();
		Result<void> lowerSetpoint();
		std::vector<std::string> commandNames() const override { return { "raiseSetpoint", "lowerSetpoint" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		double _minimumSetpoint = 0.0;
		double _maximumSetpoint = 0.0;
	};

} // namespace custom
} // namespace smartthings4cpp
