#pragma once

#include "../capability.h"

/**
 * @file temperature_measurement.h
 * @brief "temperatureMeasurement" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c temperatureMeasurement capability: measured temperature
	 *
	 * Attribute: @c temperature ({value: number, unit: "C"|"F"}). No commands.
	 */
	class TemperatureMeasurement : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "temperatureMeasurement";

		TemperatureMeasurement(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief Measured temperature value (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<double> Temperature{
			[this]() { return _temperature; }
		};

		/** @brief Temperature unit, "C" or "F" (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> Unit{
			[this]() { return _unit; }
		};

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		double _temperature = 0.0;
		std::string _unit;
	};

} // namespace smartthings4cpp
