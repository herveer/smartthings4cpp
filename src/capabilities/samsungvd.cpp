#include "smartthings4cpp/capabilities/samsungvd.h"

#include <utility>

namespace smartthings4cpp {
namespace samsungvd {

	Ambient::Ambient(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.ambient", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Ambient::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("info"); if (!v.is_null()) SetPropertyValueAndNotify<&Ambient::Info>(_info, v); }
	}

	Result<void> Ambient::sendData(const nlohmann::json& data) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(data);
		return executeCommand("sendData", commandArgs);
	}

	Result<void> Ambient::setAmbientOn() {
		return executeCommand("setAmbientOn");
	}

	AmbientContent::AmbientContent(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.ambientContent", version, std::move(componentId), std::move(deviceId), client) {
	}

	void AmbientContent::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("supportedAmbientApps"); if (!v.is_null()) SetPropertyValueAndNotify<&AmbientContent::SupportedAmbientApps>(_supportedAmbientApps, v); }
	}

	Result<void> AmbientContent::setAmbientContent(const std::string& id) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(id);
		return executeCommand("setAmbientContent", commandArgs);
	}

	Art::Art(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.art", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> Art::setArtOn() {
		return executeCommand("setArtOn");
	}

	AudioGroupInfo::AudioGroupInfo(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.audioGroupInfo", version, std::move(componentId), std::move(deviceId), client) {
	}

	void AudioGroupInfo::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&AudioGroupInfo::Role>(_role, attributeString("role", _role));
		SetPropertyValueAndNotify<&AudioGroupInfo::Channel>(_channel, attributeString("channel", _channel));
		SetPropertyValueAndNotify<&AudioGroupInfo::Status>(_status, attributeString("status", _status));
	}

	AudioInputSource::AudioInputSource(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.audioInputSource", version, std::move(componentId), std::move(deviceId), client) {
	}

	void AudioInputSource::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedInputSources"); if (!v.empty()) SetPropertyValueAndNotify<&AudioInputSource::SupportedInputSources>(_supportedInputSources, v); }
		SetPropertyValueAndNotify<&AudioInputSource::InputSource>(_inputSource, attributeString("inputSource", _inputSource));
	}

	Result<void> AudioInputSource::setNextInputSource() {
		return executeCommand("setNextInputSource");
	}

	DeviceCategory::DeviceCategory(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.deviceCategory", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DeviceCategory::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DeviceCategory::CategoryAttr>(_categoryAttr, attributeString("category", _categoryAttr));
	}

	FirmwareVersion::FirmwareVersion(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.firmwareVersion", version, std::move(componentId), std::move(deviceId), client) {
	}

	void FirmwareVersion::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&FirmwareVersion::FirmwareVersionValue>(_firmwareVersionValue, attributeString("firmwareVersion", _firmwareVersionValue));
	}

	Result<void> FirmwareVersion::setFirmwareVersion(const std::string& version) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(version);
		return executeCommand("setFirmwareVersion", commandArgs);
	}

	LaunchService::LaunchService(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.launchService", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> LaunchService::launchTVPlus(const std::string& providerId, const std::string& serviceId) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(providerId);
		commandArgs.push_back(serviceId);
		return executeCommand("launchTVPlus", commandArgs);
	}

	LightControl::LightControl(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.lightControl", version, std::move(componentId), std::move(deviceId), client) {
	}

	void LightControl::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("supportedModeMap"); if (!v.is_null()) SetPropertyValueAndNotify<&LightControl::SupportedModeMap>(_supportedModeMap, v); }
		SetPropertyValueAndNotify<&LightControl::RequestId>(_requestId, attributeString("requestId", _requestId));
		SetPropertyValueAndNotify<&LightControl::SelectedMode>(_selectedMode, attributeString("selectedMode", _selectedMode));
		SetPropertyValueAndNotify<&LightControl::StreamControl>(_streamControl, attributeBool("streamControl", _streamControl));
		SetPropertyValueAndNotify<&LightControl::SelectedAppId>(_selectedAppId, attributeString("selectedAppId", _selectedAppId));
		SetPropertyValueAndNotify<&LightControl::ErrorCode>(_errorCode, attributeString("errorCode", _errorCode));
		{ auto v = attributeValue("supportedModes"); if (!v.is_null()) SetPropertyValueAndNotify<&LightControl::SupportedModes>(_supportedModes, v); }
	}

	Result<void> LightControl::setLightControlMode(const std::string& selectedMode) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(selectedMode);
		return executeCommand("setLightControlMode", commandArgs);
	}

	MediaInputSource::MediaInputSource(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.mediaInputSource", version, std::move(componentId), std::move(deviceId), client) {
	}

	void MediaInputSource::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("supportedInputSourcesMap"); if (!v.is_null()) SetPropertyValueAndNotify<&MediaInputSource::SupportedInputSourcesMap>(_supportedInputSourcesMap, v); }
		SetPropertyValueAndNotify<&MediaInputSource::InputSource>(_inputSource, attributeString("inputSource", _inputSource));
	}

	Result<void> MediaInputSource::setInputSource(const std::string& id) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(id);
		return executeCommand("setInputSource", commandArgs);
	}

	RemoteControl::RemoteControl(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.remoteControl", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> RemoteControl::send(const std::string& keyValue, const std::string& keyState) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(keyValue);
		commandArgs.push_back(keyState);
		return executeCommand("send", commandArgs);
	}

	SoundDetection::SoundDetection(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.soundDetection", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SoundDetection::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&SoundDetection::SoundDetected>(_soundDetected, attributeString("soundDetected", _soundDetected));
	}

	SoundFrom::SoundFrom(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.soundFrom", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SoundFrom::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&SoundFrom::Mode>(_mode, static_cast<int>(attributeNumber("mode", _mode)));
		SetPropertyValueAndNotify<&SoundFrom::DetailName>(_detailName, attributeString("detailName", _detailName));
	}

	Result<void> SoundFrom::setSoundFrom(int mode, const std::string& detailName) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(mode);
		commandArgs.push_back(detailName);
		return executeCommand("setSoundFrom", commandArgs);
	}

	SupportsFeatures::SupportsFeatures(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.supportsFeatures", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SupportsFeatures::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&SupportsFeatures::MediaOutputSupported>(_mediaOutputSupported, attributeBool("mediaOutputSupported", _mediaOutputSupported));
		SetPropertyValueAndNotify<&SupportsFeatures::ImeAdvSupported>(_imeAdvSupported, attributeBool("imeAdvSupported", _imeAdvSupported));
		SetPropertyValueAndNotify<&SupportsFeatures::WifiUpdateSupport>(_wifiUpdateSupport, attributeBool("wifiUpdateSupport", _wifiUpdateSupport));
		{ auto v = attributeStringArray("executableServiceList"); if (!v.empty()) SetPropertyValueAndNotify<&SupportsFeatures::ExecutableServiceList>(_executableServiceList, v); }
		SetPropertyValueAndNotify<&SupportsFeatures::RemotelessSupported>(_remotelessSupported, attributeBool("remotelessSupported", _remotelessSupported));
		SetPropertyValueAndNotify<&SupportsFeatures::ArtSupported>(_artSupported, attributeBool("artSupported", _artSupported));
		SetPropertyValueAndNotify<&SupportsFeatures::MobileCamSupported>(_mobileCamSupported, attributeBool("mobileCamSupported", _mobileCamSupported));
	}

	SupportsPowerOnByOcf::SupportsPowerOnByOcf(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.supportsPowerOnByOcf", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SupportsPowerOnByOcf::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&SupportsPowerOnByOcf::SupportsPowerOnByOcfValue>(_supportsPowerOnByOcfValue, attributeString("supportsPowerOnByOcf", _supportsPowerOnByOcfValue));
	}

	ThingStatus::ThingStatus(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungvd.thingStatus", version, std::move(componentId), std::move(deviceId), client) {
	}

	void ThingStatus::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&ThingStatus::UpdatedTime>(_updatedTime, static_cast<int>(attributeNumber("updatedTime", _updatedTime)));
		SetPropertyValueAndNotify<&ThingStatus::Status>(_status, attributeString("status", _status));
	}

} // namespace samsungvd
} // namespace smartthings4cpp
