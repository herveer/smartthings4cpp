#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.dustFilter capability (Samsung-proprietary; generated). */
	class DustFilter : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.dustFilter";

		DustFilter(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> DustFilterUsageStep = MakeReadonlyProperty<&DustFilter::DustFilterUsageStep>([this]() { return _dustFilterUsageStep; });
		ReactiveLitepp::ReadonlyProperty<int> DustFilterUsage = MakeReadonlyProperty<&DustFilter::DustFilterUsage>([this]() { return _dustFilterUsage; });
		ReactiveLitepp::ReadonlyProperty<std::string> DustFilterLastResetDate = MakeReadonlyProperty<&DustFilter::DustFilterLastResetDate>([this]() { return _dustFilterLastResetDate; });
		ReactiveLitepp::ReadonlyProperty<std::string> DustFilterStatus = MakeReadonlyProperty<&DustFilter::DustFilterStatus>([this]() { return _dustFilterStatus; });
		ReactiveLitepp::ReadonlyProperty<int> DustFilterCapacity = MakeReadonlyProperty<&DustFilter::DustFilterCapacity>([this]() { return _dustFilterCapacity; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> DustFilterResetType = MakeReadonlyProperty<&DustFilter::DustFilterResetType>([this]() { return _dustFilterResetType; });

		Result<void> resetDustFilter();
		std::vector<std::string> commandNames() const override { return { "resetDustFilter" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _dustFilterUsageStep = 0;
		int _dustFilterUsage = 0;
		std::string _dustFilterLastResetDate;
		std::string _dustFilterStatus;
		int _dustFilterCapacity = 0;
		std::vector<std::string> _dustFilterResetType;
	};

} // namespace custom
} // namespace smartthings4cpp
