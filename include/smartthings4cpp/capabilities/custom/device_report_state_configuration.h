#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.deviceReportStateConfiguration capability (Samsung-proprietary; generated). */
	class DeviceReportStateConfiguration : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.deviceReportStateConfiguration";

		DeviceReportStateConfiguration(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> ReportStateRealtimePeriod{ [this]() { return _reportStateRealtimePeriod; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ReportStateRealtime{ [this]() { return _reportStateRealtime; } };
		ReactiveLitepp::ReadonlyProperty<std::string> ReportStatePeriod{ [this]() { return _reportStatePeriod; } };

		Result<void> setReportStatePeriod(const std::string& value);
		Result<void> setReportStateRealtime(const nlohmann::json& value);
		Result<void> setReportStateRealtimePeriod(const std::string& value);
		std::vector<std::string> commandNames() const override { return { "setReportStatePeriod", "setReportStateRealtime", "setReportStateRealtimePeriod" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _reportStateRealtimePeriod;
		nlohmann::json _reportStateRealtime;
		std::string _reportStatePeriod;
	};

} // namespace custom
} // namespace smartthings4cpp
