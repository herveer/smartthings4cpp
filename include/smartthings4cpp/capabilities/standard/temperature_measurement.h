#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c temperatureMeasurement capability (generated). */
	class TemperatureMeasurement : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "temperatureMeasurement";

		TemperatureMeasurement(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> TemperatureRange{ [this]() { return _temperatureRange; } };
		ReactiveLitepp::ReadonlyProperty<double> Temperature{ [this]() { return _temperature; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _temperatureRange;
		double _temperature = 0.0;
	};

} // namespace standard
} // namespace smartthings4cpp
