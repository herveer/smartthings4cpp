#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.waterFilter capability (Samsung-proprietary; generated). */
	class WaterFilter : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.waterFilter";

		WaterFilter(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> WaterFilterUsageStep = MakeReadonlyProperty<&WaterFilter::WaterFilterUsageStep>([this]() { return _waterFilterUsageStep; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> WaterFilterResetType = MakeReadonlyProperty<&WaterFilter::WaterFilterResetType>([this]() { return _waterFilterResetType; });
		ReactiveLitepp::ReadonlyProperty<int> WaterFilterCapacity = MakeReadonlyProperty<&WaterFilter::WaterFilterCapacity>([this]() { return _waterFilterCapacity; });
		ReactiveLitepp::ReadonlyProperty<std::string> WaterFilterLastResetDate = MakeReadonlyProperty<&WaterFilter::WaterFilterLastResetDate>([this]() { return _waterFilterLastResetDate; });
		ReactiveLitepp::ReadonlyProperty<int> WaterFilterUsage = MakeReadonlyProperty<&WaterFilter::WaterFilterUsage>([this]() { return _waterFilterUsage; });
		ReactiveLitepp::ReadonlyProperty<std::string> WaterFilterStatus = MakeReadonlyProperty<&WaterFilter::WaterFilterStatus>([this]() { return _waterFilterStatus; });

		Result<void> resetWaterFilter();
		std::vector<std::string> commandNames() const override { return { "resetWaterFilter" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _waterFilterUsageStep = 0;
		std::vector<std::string> _waterFilterResetType;
		int _waterFilterCapacity = 0;
		std::string _waterFilterLastResetDate;
		int _waterFilterUsage = 0;
		std::string _waterFilterStatus;
	};

} // namespace custom
} // namespace smartthings4cpp
