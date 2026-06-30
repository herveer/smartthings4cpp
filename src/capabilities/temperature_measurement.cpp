#include "smartthings4cpp/capabilities/temperature_measurement.h"

#include <utility>

namespace smartthings4cpp {

	TemperatureMeasurement::TemperatureMeasurement(int version, std::string componentId,
		std::string deviceId, Client* client)
		: Capability("temperatureMeasurement", version, std::move(componentId), std::move(deviceId), client) {
	}

	void TemperatureMeasurement::parseStatus(const nlohmann::json&) {
		double temperature = attributeNumber("temperature", _temperature);
		SetPropertyValueAndNotify<&TemperatureMeasurement::Temperature>(_temperature, temperature);

		std::string unit = attributeUnit("temperature", _unit);
		SetPropertyValueAndNotify<&TemperatureMeasurement::Unit>(_unit, unit);
	}

} // namespace smartthings4cpp
