#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.waterConsumptionReport capability (Samsung-proprietary; generated). */
	class WaterConsumptionReport : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.waterConsumptionReport";

		WaterConsumptionReport(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> WaterConsumption = MakeReadonlyProperty<&WaterConsumptionReport::WaterConsumption>([this]() { return _waterConsumption; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _waterConsumption;
	};

} // namespace samsungce
} // namespace smartthings4cpp
