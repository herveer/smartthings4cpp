#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.deodorFilter capability (Samsung-proprietary; generated). */
	class DeodorFilter : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.deodorFilter";

		DeodorFilter(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> DeodorFilterCapacity = MakeReadonlyProperty<&DeodorFilter::DeodorFilterCapacity>([this]() { return _deodorFilterCapacity; });
		ReactiveLitepp::ReadonlyProperty<std::string> DeodorFilterLastResetDate = MakeReadonlyProperty<&DeodorFilter::DeodorFilterLastResetDate>([this]() { return _deodorFilterLastResetDate; });
		ReactiveLitepp::ReadonlyProperty<std::string> DeodorFilterStatus = MakeReadonlyProperty<&DeodorFilter::DeodorFilterStatus>([this]() { return _deodorFilterStatus; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> DeodorFilterResetType = MakeReadonlyProperty<&DeodorFilter::DeodorFilterResetType>([this]() { return _deodorFilterResetType; });
		ReactiveLitepp::ReadonlyProperty<int> DeodorFilterUsage = MakeReadonlyProperty<&DeodorFilter::DeodorFilterUsage>([this]() { return _deodorFilterUsage; });
		ReactiveLitepp::ReadonlyProperty<int> DeodorFilterUsageStep = MakeReadonlyProperty<&DeodorFilter::DeodorFilterUsageStep>([this]() { return _deodorFilterUsageStep; });

		Result<void> resetDeodorFilter();
		std::vector<std::string> commandNames() const override { return { "resetDeodorFilter" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _deodorFilterCapacity = 0;
		std::string _deodorFilterLastResetDate;
		std::string _deodorFilterStatus;
		std::vector<std::string> _deodorFilterResetType;
		int _deodorFilterUsage = 0;
		int _deodorFilterUsageStep = 0;
	};

} // namespace custom
} // namespace smartthings4cpp
