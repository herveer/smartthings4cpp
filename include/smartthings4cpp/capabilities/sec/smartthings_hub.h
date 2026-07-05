#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace sec {

	/** @brief The @c sec.smartthingsHub capability (Samsung-proprietary; generated). */
	class SmartthingsHub : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "sec.smartthingsHub";

		SmartthingsHub(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> ThreadHardwareAvailability = MakeReadonlyProperty<&SmartthingsHub::ThreadHardwareAvailability>([this]() { return _threadHardwareAvailability; });
		ReactiveLitepp::ReadonlyProperty<std::string> Availability = MakeReadonlyProperty<&SmartthingsHub::Availability>([this]() { return _availability; });
		ReactiveLitepp::ReadonlyProperty<std::string> DeviceId = MakeReadonlyProperty<&SmartthingsHub::DeviceId>([this]() { return _deviceId; });
		ReactiveLitepp::ReadonlyProperty<std::string> ZigbeeHardwareAvailability = MakeReadonlyProperty<&SmartthingsHub::ZigbeeHardwareAvailability>([this]() { return _zigbeeHardwareAvailability; });
		ReactiveLitepp::ReadonlyProperty<std::string> VersionAttr = MakeReadonlyProperty<&SmartthingsHub::VersionAttr>([this]() { return _versionAttr; });
		ReactiveLitepp::ReadonlyProperty<bool> ThreadRequiresExternalHardware = MakeReadonlyProperty<&SmartthingsHub::ThreadRequiresExternalHardware>([this]() { return _threadRequiresExternalHardware; });
		ReactiveLitepp::ReadonlyProperty<bool> ZigbeeRequiresExternalHardware = MakeReadonlyProperty<&SmartthingsHub::ZigbeeRequiresExternalHardware>([this]() { return _zigbeeRequiresExternalHardware; });
		ReactiveLitepp::ReadonlyProperty<std::string> Eui = MakeReadonlyProperty<&SmartthingsHub::Eui>([this]() { return _eui; });
		ReactiveLitepp::ReadonlyProperty<std::string> LastOnboardingResult = MakeReadonlyProperty<&SmartthingsHub::LastOnboardingResult>([this]() { return _lastOnboardingResult; });
		ReactiveLitepp::ReadonlyProperty<std::string> ZwaveHardwareAvailability = MakeReadonlyProperty<&SmartthingsHub::ZwaveHardwareAvailability>([this]() { return _zwaveHardwareAvailability; });
		ReactiveLitepp::ReadonlyProperty<bool> ZwaveRequiresExternalHardware = MakeReadonlyProperty<&SmartthingsHub::ZwaveRequiresExternalHardware>([this]() { return _zwaveRequiresExternalHardware; });
		ReactiveLitepp::ReadonlyProperty<std::string> State = MakeReadonlyProperty<&SmartthingsHub::State>([this]() { return _state; });
		ReactiveLitepp::ReadonlyProperty<int> OnboardingProgress = MakeReadonlyProperty<&SmartthingsHub::OnboardingProgress>([this]() { return _onboardingProgress; });
		ReactiveLitepp::ReadonlyProperty<std::string> LastOnboardingErrorCode = MakeReadonlyProperty<&SmartthingsHub::LastOnboardingErrorCode>([this]() { return _lastOnboardingErrorCode; });

		Result<void> onboarding(const std::string& locationId, const std::string& roomId);
		Result<void> cancelOnboarding();
		std::vector<std::string> commandNames() const override { return { "onboarding", "cancelOnboarding" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _threadHardwareAvailability;
		std::string _availability;
		std::string _deviceId;
		std::string _zigbeeHardwareAvailability;
		std::string _versionAttr;
		bool _threadRequiresExternalHardware = false;
		bool _zigbeeRequiresExternalHardware = false;
		std::string _eui;
		std::string _lastOnboardingResult;
		std::string _zwaveHardwareAvailability;
		bool _zwaveRequiresExternalHardware = false;
		std::string _state;
		int _onboardingProgress = 0;
		std::string _lastOnboardingErrorCode;
	};

} // namespace sec
} // namespace smartthings4cpp
