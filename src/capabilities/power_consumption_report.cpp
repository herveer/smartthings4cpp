#include "smartthings4cpp/capabilities/power_consumption_report.h"

#include <utility>

namespace smartthings4cpp {

	PowerConsumptionReport::PowerConsumptionReport(int version, std::string componentId,
		std::string deviceId, Client* client)
		: Capability("powerConsumptionReport", version, std::move(componentId), std::move(deviceId), client) {
	}

	void PowerConsumptionReport::parseStatus(const nlohmann::json&) {
		auto value = attributeValue("powerConsumption");
		if (!value.is_object()) {
			return;
		}

		auto number = [&](const char* key, double fallback) {
			return (value.contains(key) && value[key].is_number())
				? value[key].get<double>() : fallback;
		};

		SetPropertyValueAndNotify<&PowerConsumptionReport::Power>(_power, number("power", _power));
		SetPropertyValueAndNotify<&PowerConsumptionReport::Energy>(_energy, number("energy", _energy));
		SetPropertyValueAndNotify<&PowerConsumptionReport::DeltaEnergy>(_deltaEnergy, number("deltaEnergy", _deltaEnergy));
	}

} // namespace smartthings4cpp
