#include "smartthings4cpp/capabilities/sec.h"

#include <utility>

namespace smartthings4cpp {
namespace sec {

	DiagnosticsInformation::DiagnosticsInformation(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("sec.diagnosticsInformation", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DiagnosticsInformation::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("logType"); if (!v.empty()) SetPropertyValueAndNotify<&DiagnosticsInformation::LogType>(_logType, v); }
		SetPropertyValueAndNotify<&DiagnosticsInformation::Endpoint>(_endpoint, attributeString("endpoint", _endpoint));
		SetPropertyValueAndNotify<&DiagnosticsInformation::MinVersion>(_minVersion, attributeString("minVersion", _minVersion));
		SetPropertyValueAndNotify<&DiagnosticsInformation::SigninPermission>(_signinPermission, attributeString("signinPermission", _signinPermission));
		SetPropertyValueAndNotify<&DiagnosticsInformation::SetupId>(_setupId, attributeString("setupId", _setupId));
		SetPropertyValueAndNotify<&DiagnosticsInformation::ProtocolType>(_protocolType, attributeString("protocolType", _protocolType));
		SetPropertyValueAndNotify<&DiagnosticsInformation::TsId>(_tsId, attributeString("tsId", _tsId));
		SetPropertyValueAndNotify<&DiagnosticsInformation::MnId>(_mnId, attributeString("mnId", _mnId));
		SetPropertyValueAndNotify<&DiagnosticsInformation::DumpType>(_dumpType, attributeString("dumpType", _dumpType));
	}

	SmartthingsHub::SmartthingsHub(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("sec.smartthingsHub", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SmartthingsHub::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&SmartthingsHub::ThreadHardwareAvailability>(_threadHardwareAvailability, attributeString("threadHardwareAvailability", _threadHardwareAvailability));
		SetPropertyValueAndNotify<&SmartthingsHub::Availability>(_availability, attributeString("availability", _availability));
		SetPropertyValueAndNotify<&SmartthingsHub::DeviceId>(_deviceId, attributeString("deviceId", _deviceId));
		SetPropertyValueAndNotify<&SmartthingsHub::ZigbeeHardwareAvailability>(_zigbeeHardwareAvailability, attributeString("zigbeeHardwareAvailability", _zigbeeHardwareAvailability));
		SetPropertyValueAndNotify<&SmartthingsHub::VersionAttr>(_versionAttr, attributeString("version", _versionAttr));
		SetPropertyValueAndNotify<&SmartthingsHub::ThreadRequiresExternalHardware>(_threadRequiresExternalHardware, attributeBool("threadRequiresExternalHardware", _threadRequiresExternalHardware));
		SetPropertyValueAndNotify<&SmartthingsHub::ZigbeeRequiresExternalHardware>(_zigbeeRequiresExternalHardware, attributeBool("zigbeeRequiresExternalHardware", _zigbeeRequiresExternalHardware));
		SetPropertyValueAndNotify<&SmartthingsHub::Eui>(_eui, attributeString("eui", _eui));
		SetPropertyValueAndNotify<&SmartthingsHub::LastOnboardingResult>(_lastOnboardingResult, attributeString("lastOnboardingResult", _lastOnboardingResult));
		SetPropertyValueAndNotify<&SmartthingsHub::ZwaveHardwareAvailability>(_zwaveHardwareAvailability, attributeString("zwaveHardwareAvailability", _zwaveHardwareAvailability));
		SetPropertyValueAndNotify<&SmartthingsHub::ZwaveRequiresExternalHardware>(_zwaveRequiresExternalHardware, attributeBool("zwaveRequiresExternalHardware", _zwaveRequiresExternalHardware));
		SetPropertyValueAndNotify<&SmartthingsHub::State>(_state, attributeString("state", _state));
		SetPropertyValueAndNotify<&SmartthingsHub::OnboardingProgress>(_onboardingProgress, static_cast<int>(attributeNumber("onboardingProgress", _onboardingProgress)));
		SetPropertyValueAndNotify<&SmartthingsHub::LastOnboardingErrorCode>(_lastOnboardingErrorCode, attributeString("lastOnboardingErrorCode", _lastOnboardingErrorCode));
	}

	Result<void> SmartthingsHub::onboarding(const std::string& locationId, const std::string& roomId) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(locationId);
		commandArgs.push_back(roomId);
		return executeCommand("onboarding", commandArgs);
	}

	Result<void> SmartthingsHub::cancelOnboarding() {
		return executeCommand("cancelOnboarding");
	}

	WifiConfiguration::WifiConfiguration(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("sec.wifiConfiguration", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WifiConfiguration::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WifiConfiguration::AutoReconnection>(_autoReconnection, attributeBool("autoReconnection", _autoReconnection));
		SetPropertyValueAndNotify<&WifiConfiguration::MinVersion>(_minVersion, attributeString("minVersion", _minVersion));
		{ auto v = attributeStringArray("supportedWiFiFreq"); if (!v.empty()) SetPropertyValueAndNotify<&WifiConfiguration::SupportedWiFiFreq>(_supportedWiFiFreq, v); }
		{ auto v = attributeStringArray("supportedAuthType"); if (!v.empty()) SetPropertyValueAndNotify<&WifiConfiguration::SupportedAuthType>(_supportedAuthType, v); }
		{ auto v = attributeStringArray("protocolType"); if (!v.empty()) SetPropertyValueAndNotify<&WifiConfiguration::ProtocolType>(_protocolType, v); }
	}

} // namespace sec
} // namespace smartthings4cpp
