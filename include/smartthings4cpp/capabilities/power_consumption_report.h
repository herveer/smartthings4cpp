#pragma once

#include "../capability.h"
#include <string>

/**
 * @file power_consumption_report.h
 * @brief "powerConsumptionReport" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c powerConsumptionReport capability: energy/power reporting
	 *
	 * Attribute: @c powerConsumption (an object with @c power, @c energy,
	 * @c deltaEnergy, ...). No commands. The most common fields are surfaced as
	 * typed read-only properties; the full object is available via statusJson().
	 */
	class PowerConsumptionReport : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "powerConsumptionReport";

		PowerConsumptionReport(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief Instantaneous power in watts (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<double> Power{
			[this]() { return _power; }
		};

		/** @brief Cumulative energy in watt-hours (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<double> Energy{
			[this]() { return _energy; }
		};

		/** @brief Energy consumed since the previous report, in watt-hours (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<double> DeltaEnergy{
			[this]() { return _deltaEnergy; }
		};

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		double _power = 0.0;
		double _energy = 0.0;
		double _deltaEnergy = 0.0;
	};

} // namespace smartthings4cpp
