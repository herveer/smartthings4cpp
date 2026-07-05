#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c powerConsumptionReport capability (generated). */
	class PowerConsumptionReport : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "powerConsumptionReport";

		PowerConsumptionReport(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> PowerConsumption = MakeReadonlyProperty<&PowerConsumptionReport::PowerConsumption>([this]() { return _powerConsumption; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _powerConsumption;
	};

} // namespace standard
} // namespace smartthings4cpp
