#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace sec {

	/** @brief The @c sec.smartthingsHub capability (Samsung-proprietary; generated). */
	class SmartthingsHub : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "sec.smartthingsHub";

		SmartthingsHub(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> ThreadHardwareAvailability{ [this]() { return _threadHardwareAvailability; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Availability{ [this]() { return _availability; } };
		ReactiveLitepp::ReadonlyProperty<std::string> DeviceId{ [this]() { return _deviceId; } };
		ReactiveLitepp::ReadonlyProperty<std::string> ZigbeeHardwareAvailability{ [this]() { return _zigbeeHardwareAvailability; } };
		ReactiveLitepp::ReadonlyProperty<std::string> VersionAttr{ [this]() { return _versionAttr; } };
		ReactiveLitepp::ReadonlyProperty<bool> ThreadRequiresExternalHardware{ [this]() { return _threadRequiresExternalHardware; } };
		ReactiveLitepp::ReadonlyProperty<bool> ZigbeeRequiresExternalHardware{ [this]() { return _zigbeeRequiresExternalHardware; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Eui{ [this]() { return _eui; } };
		ReactiveLitepp::ReadonlyProperty<std::string> LastOnboardingResult{ [this]() { return _lastOnboardingResult; } };
		ReactiveLitepp::ReadonlyProperty<std::string> ZwaveHardwareAvailability{ [this]() { return _zwaveHardwareAvailability; } };
		ReactiveLitepp::ReadonlyProperty<bool> ZwaveRequiresExternalHardware{ [this]() { return _zwaveRequiresExternalHardware; } };
		ReactiveLitepp::ReadonlyProperty<std::string> State{ [this]() { return _state; } };
		ReactiveLitepp::ReadonlyProperty<int> OnboardingProgress{ [this]() { return _onboardingProgress; } };
		ReactiveLitepp::ReadonlyProperty<std::string> LastOnboardingErrorCode{ [this]() { return _lastOnboardingErrorCode; } };

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
