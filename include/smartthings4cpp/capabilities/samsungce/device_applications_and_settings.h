#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.deviceApplicationsAndSettings capability (Samsung-proprietary; generated). */
	class DeviceApplicationsAndSettings : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.deviceApplicationsAndSettings";

		DeviceApplicationsAndSettings(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> DeviceApplications = MakeReadonlyProperty<&DeviceApplicationsAndSettings::DeviceApplications>([this]() { return _deviceApplications; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> DeviceSettings = MakeReadonlyProperty<&DeviceApplicationsAndSettings::DeviceSettings>([this]() { return _deviceSettings; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _deviceApplications;
		nlohmann::json _deviceSettings;
	};

} // namespace samsungce
} // namespace smartthings4cpp
