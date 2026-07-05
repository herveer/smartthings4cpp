#include "smartthings4cpp/capabilities/standard.h"

#include <utility>

namespace smartthings4cpp {
namespace standard {

	AudioMute::AudioMute(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("audioMute", version, std::move(componentId), std::move(deviceId), client) {
	}

	void AudioMute::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&AudioMute::Mute>(_mute, attributeString("mute", _mute));
	}

	Result<void> AudioMute::setMute(const std::string& state) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(state);
		return executeCommand("setMute", commandArgs);
	}

	Result<void> AudioMute::mute() {
		return executeCommand("mute");
	}

	Result<void> AudioMute::unmute() {
		return executeCommand("unmute");
	}

	AudioNotification::AudioNotification(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("audioNotification", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> AudioNotification::playTrackAndResume(const std::string& uri, int level) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(uri);
		commandArgs.push_back(level);
		return executeCommand("playTrackAndResume", commandArgs);
	}

	Result<void> AudioNotification::playTrackAndRestore(const std::string& uri, int level) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(uri);
		commandArgs.push_back(level);
		return executeCommand("playTrackAndRestore", commandArgs);
	}

	Result<void> AudioNotification::playTrack(const std::string& uri, int level) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(uri);
		commandArgs.push_back(level);
		return executeCommand("playTrack", commandArgs);
	}

	AudioTrackData::AudioTrackData(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("audioTrackData", version, std::move(componentId), std::move(deviceId), client) {
	}

	void AudioTrackData::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&AudioTrackData::TotalTime>(_totalTime, static_cast<int>(attributeNumber("totalTime", _totalTime)));
		{ auto v = attributeValue("audioTrackData"); if (!v.is_null()) SetPropertyValueAndNotify<&AudioTrackData::AudioTrackDataValue>(_audioTrackDataValue, v); }
		SetPropertyValueAndNotify<&AudioTrackData::ElapsedTime>(_elapsedTime, static_cast<int>(attributeNumber("elapsedTime", _elapsedTime)));
	}

	AudioVolume::AudioVolume(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("audioVolume", version, std::move(componentId), std::move(deviceId), client) {
	}

	void AudioVolume::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&AudioVolume::Volume>(_volume, static_cast<int>(attributeNumber("volume", _volume)));
	}

	Result<void> AudioVolume::volumeDown() {
		return executeCommand("volumeDown");
	}

	Result<void> AudioVolume::volumeUp() {
		return executeCommand("volumeUp");
	}

	Result<void> AudioVolume::setVolume(int volume) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(volume);
		return executeCommand("setVolume", commandArgs);
	}

	Battery::Battery(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("battery", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Battery::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&Battery::Quantity>(_quantity, static_cast<int>(attributeNumber("quantity", _quantity)));
		SetPropertyValueAndNotify<&Battery::BatteryValue>(_batteryValue, static_cast<int>(attributeNumber("battery", _batteryValue)));
		SetPropertyValueAndNotify<&Battery::Type>(_type, attributeString("type", _type));
	}

	Bridge::Bridge(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("bridge", version, std::move(componentId), std::move(deviceId), client) {
	}

	ColorControl::ColorControl(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("colorControl", version, std::move(componentId), std::move(deviceId), client) {
	}

	void ColorControl::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&ColorControl::Saturation>(_saturation, attributeNumber("saturation", _saturation));
		SetPropertyValueAndNotify<&ColorControl::Color>(_color, attributeString("color", _color));
		SetPropertyValueAndNotify<&ColorControl::Hue>(_hue, attributeNumber("hue", _hue));
	}

	Result<void> ColorControl::setColor(const nlohmann::json& color) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(color);
		return executeCommand("setColor", commandArgs);
	}

	Result<void> ColorControl::setHue(double hue) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(hue);
		return executeCommand("setHue", commandArgs);
	}

	Result<void> ColorControl::setSaturation(double saturation) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(saturation);
		return executeCommand("setSaturation", commandArgs);
	}

	ColorTemperature::ColorTemperature(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("colorTemperature", version, std::move(componentId), std::move(deviceId), client) {
	}

	void ColorTemperature::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("colorTemperatureRange"); if (!v.is_null()) SetPropertyValueAndNotify<&ColorTemperature::ColorTemperatureRange>(_colorTemperatureRange, v); }
		SetPropertyValueAndNotify<&ColorTemperature::ColorTemperatureValue>(_colorTemperatureValue, static_cast<int>(attributeNumber("colorTemperature", _colorTemperatureValue)));
	}

	Result<void> ColorTemperature::setColorTemperature(int temperature) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(temperature);
		return executeCommand("setColorTemperature", commandArgs);
	}

	ContactSensor::ContactSensor(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("contactSensor", version, std::move(componentId), std::move(deviceId), client) {
	}

	void ContactSensor::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&ContactSensor::Contact>(_contact, attributeString("contact", _contact));
	}

	DemandResponseLoadControl::DemandResponseLoadControl(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("demandResponseLoadControl", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DemandResponseLoadControl::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("drlcStatus"); if (!v.is_null()) SetPropertyValueAndNotify<&DemandResponseLoadControl::DrlcStatus>(_drlcStatus, v); }
	}

	Result<void> DemandResponseLoadControl::overrideDrlcAction(bool value) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(value);
		return executeCommand("overrideDrlcAction", commandArgs);
	}

	Result<void> DemandResponseLoadControl::requestDrlcAction(int drlcType, int drlcLevel, const std::string& start, int duration, int reportingPeriod) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(drlcType);
		commandArgs.push_back(drlcLevel);
		commandArgs.push_back(start);
		commandArgs.push_back(duration);
		commandArgs.push_back(reportingPeriod);
		return executeCommand("requestDrlcAction", commandArgs);
	}

	DishwasherOperatingState::DishwasherOperatingState(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("dishwasherOperatingState", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DishwasherOperatingState::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DishwasherOperatingState::CompletionTime>(_completionTime, attributeString("completionTime", _completionTime));
		SetPropertyValueAndNotify<&DishwasherOperatingState::MachineState>(_machineState, attributeString("machineState", _machineState));
		SetPropertyValueAndNotify<&DishwasherOperatingState::Progress>(_progress, static_cast<int>(attributeNumber("progress", _progress)));
		{ auto v = attributeStringArray("supportedMachineStates"); if (!v.empty()) SetPropertyValueAndNotify<&DishwasherOperatingState::SupportedMachineStates>(_supportedMachineStates, v); }
		SetPropertyValueAndNotify<&DishwasherOperatingState::DishwasherJobState>(_dishwasherJobState, attributeString("dishwasherJobState", _dishwasherJobState));
	}

	Result<void> DishwasherOperatingState::setMachineState(const std::string& state) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(state);
		return executeCommand("setMachineState", commandArgs);
	}

	Execute::Execute(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("execute", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Execute::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("data"); if (!v.is_null()) SetPropertyValueAndNotify<&Execute::Data>(_data, v); }
	}

	Result<void> Execute::execute(const std::string& command, const nlohmann::json& args) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(command);
		commandArgs.push_back(args);
		return executeCommand("execute", commandArgs);
	}

	HealthCheck::HealthCheck(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("healthCheck", version, std::move(componentId), std::move(deviceId), client) {
	}

	void HealthCheck::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&HealthCheck::CheckInterval>(_checkInterval, static_cast<int>(attributeNumber("checkInterval", _checkInterval)));
		SetPropertyValueAndNotify<&HealthCheck::HealthStatus>(_healthStatus, attributeString("healthStatus", _healthStatus));
		{ auto v = attributeValue("DeviceWatch-Enroll"); if (!v.is_null()) SetPropertyValueAndNotify<&HealthCheck::DeviceWatchEnroll>(_deviceWatchEnroll, v); }
		SetPropertyValueAndNotify<&HealthCheck::DeviceWatchDeviceStatus>(_deviceWatchDeviceStatus, attributeString("DeviceWatch-DeviceStatus", _deviceWatchDeviceStatus));
	}

	Result<void> HealthCheck::ping() {
		return executeCommand("ping");
	}

	IlluminanceMeasurement::IlluminanceMeasurement(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("illuminanceMeasurement", version, std::move(componentId), std::move(deviceId), client) {
	}

	void IlluminanceMeasurement::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&IlluminanceMeasurement::Illuminance>(_illuminance, attributeNumber("illuminance", _illuminance));
	}

	MediaInputSource::MediaInputSource(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("mediaInputSource", version, std::move(componentId), std::move(deviceId), client) {
	}

	void MediaInputSource::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedInputSources"); if (!v.empty()) SetPropertyValueAndNotify<&MediaInputSource::SupportedInputSources>(_supportedInputSources, v); }
		SetPropertyValueAndNotify<&MediaInputSource::InputSource>(_inputSource, attributeString("inputSource", _inputSource));
	}

	Result<void> MediaInputSource::setInputSource(const std::string& mode) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(mode);
		return executeCommand("setInputSource", commandArgs);
	}

	MediaPlayback::MediaPlayback(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("mediaPlayback", version, std::move(componentId), std::move(deviceId), client) {
	}

	void MediaPlayback::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedPlaybackCommands"); if (!v.empty()) SetPropertyValueAndNotify<&MediaPlayback::SupportedPlaybackCommands>(_supportedPlaybackCommands, v); }
		SetPropertyValueAndNotify<&MediaPlayback::PlaybackStatus>(_playbackStatus, attributeString("playbackStatus", _playbackStatus));
	}

	Result<void> MediaPlayback::play() {
		return executeCommand("play");
	}

	Result<void> MediaPlayback::stop() {
		return executeCommand("stop");
	}

	Result<void> MediaPlayback::rewind() {
		return executeCommand("rewind");
	}

	Result<void> MediaPlayback::fastForward() {
		return executeCommand("fastForward");
	}

	Result<void> MediaPlayback::setPlaybackStatus(const std::string& status) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(status);
		return executeCommand("setPlaybackStatus", commandArgs);
	}

	Result<void> MediaPlayback::pause() {
		return executeCommand("pause");
	}

	MediaPresets::MediaPresets(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("mediaPresets", version, std::move(componentId), std::move(deviceId), client) {
	}

	void MediaPresets::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("presets"); if (!v.is_null()) SetPropertyValueAndNotify<&MediaPresets::Presets>(_presets, v); }
	}

	Result<void> MediaPresets::playPreset(const std::string& presetId) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(presetId);
		return executeCommand("playPreset", commandArgs);
	}

	MediaTrackControl::MediaTrackControl(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("mediaTrackControl", version, std::move(componentId), std::move(deviceId), client) {
	}

	void MediaTrackControl::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedTrackControlCommands"); if (!v.empty()) SetPropertyValueAndNotify<&MediaTrackControl::SupportedTrackControlCommands>(_supportedTrackControlCommands, v); }
	}

	Result<void> MediaTrackControl::previousTrack() {
		return executeCommand("previousTrack");
	}

	Result<void> MediaTrackControl::nextTrack() {
		return executeCommand("nextTrack");
	}

	MotionSensor::MotionSensor(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("motionSensor", version, std::move(componentId), std::move(deviceId), client) {
	}

	void MotionSensor::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&MotionSensor::Motion>(_motion, attributeString("motion", _motion));
	}

	MovementSensor::MovementSensor(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("movementSensor", version, std::move(componentId), std::move(deviceId), client) {
	}

	void MovementSensor::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedMovements"); if (!v.empty()) SetPropertyValueAndNotify<&MovementSensor::SupportedMovements>(_supportedMovements, v); }
		SetPropertyValueAndNotify<&MovementSensor::Movement>(_movement, attributeString("movement", _movement));
	}

	MultipleZonePresence::MultipleZonePresence(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("multipleZonePresence", version, std::move(componentId), std::move(deviceId), client) {
	}

	void MultipleZonePresence::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("zoneState"); if (!v.is_null()) SetPropertyValueAndNotify<&MultipleZonePresence::ZoneState>(_zoneState, v); }
	}

	Result<void> MultipleZonePresence::deleteZone(const std::string& id) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(id);
		return executeCommand("deleteZone", commandArgs);
	}

	Result<void> MultipleZonePresence::createZone(const std::string& name, const std::string& id) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		commandArgs.push_back(id);
		return executeCommand("createZone", commandArgs);
	}

	Result<void> MultipleZonePresence::updateZoneName(const std::string& id, const std::string& name) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(id);
		commandArgs.push_back(name);
		return executeCommand("updateZoneName", commandArgs);
	}

	Ocf::Ocf(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("ocf", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Ocf::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&Ocf::St>(_st, attributeString("st", _st));
		SetPropertyValueAndNotify<&Ocf::Mndt>(_mndt, attributeString("mndt", _mndt));
		SetPropertyValueAndNotify<&Ocf::Mnfv>(_mnfv, attributeString("mnfv", _mnfv));
		SetPropertyValueAndNotify<&Ocf::Mnhw>(_mnhw, attributeString("mnhw", _mnhw));
		SetPropertyValueAndNotify<&Ocf::Di>(_di, attributeString("di", _di));
		SetPropertyValueAndNotify<&Ocf::Mnsl>(_mnsl, attributeString("mnsl", _mnsl));
		SetPropertyValueAndNotify<&Ocf::Dmv>(_dmv, attributeString("dmv", _dmv));
		SetPropertyValueAndNotify<&Ocf::N>(_n, attributeString("n", _n));
		SetPropertyValueAndNotify<&Ocf::Mnmo>(_mnmo, attributeString("mnmo", _mnmo));
		SetPropertyValueAndNotify<&Ocf::Vid>(_vid, attributeString("vid", _vid));
		SetPropertyValueAndNotify<&Ocf::Mnmn>(_mnmn, attributeString("mnmn", _mnmn));
		SetPropertyValueAndNotify<&Ocf::Mnml>(_mnml, attributeString("mnml", _mnml));
		SetPropertyValueAndNotify<&Ocf::Mnpv>(_mnpv, attributeString("mnpv", _mnpv));
		SetPropertyValueAndNotify<&Ocf::Mnos>(_mnos, attributeString("mnos", _mnos));
		SetPropertyValueAndNotify<&Ocf::Pi>(_pi, attributeString("pi", _pi));
		SetPropertyValueAndNotify<&Ocf::Icv>(_icv, attributeString("icv", _icv));
	}

	Result<void> Ocf::postOcfCommand(const std::string& href, const nlohmann::json& value) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(href);
		commandArgs.push_back(value);
		return executeCommand("postOcfCommand", commandArgs);
	}

	PowerConsumptionReport::PowerConsumptionReport(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("powerConsumptionReport", version, std::move(componentId), std::move(deviceId), client) {
	}

	void PowerConsumptionReport::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("powerConsumption"); if (!v.is_null()) SetPropertyValueAndNotify<&PowerConsumptionReport::PowerConsumption>(_powerConsumption, v); }
	}

	PresenceSensor::PresenceSensor(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("presenceSensor", version, std::move(componentId), std::move(deviceId), client) {
	}

	void PresenceSensor::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&PresenceSensor::Presence>(_presence, attributeString("presence", _presence));
	}

	Refresh::Refresh(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("refresh", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> Refresh::refresh() {
		return executeCommand("refresh");
	}

	Refrigeration::Refrigeration(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("refrigeration", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Refrigeration::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&Refrigeration::Defrost>(_defrost, attributeString("defrost", _defrost));
		SetPropertyValueAndNotify<&Refrigeration::RapidCooling>(_rapidCooling, attributeString("rapidCooling", _rapidCooling));
		SetPropertyValueAndNotify<&Refrigeration::RapidFreezing>(_rapidFreezing, attributeString("rapidFreezing", _rapidFreezing));
	}

	Result<void> Refrigeration::setDefrost(const std::string& defrost) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(defrost);
		return executeCommand("setDefrost", commandArgs);
	}

	Result<void> Refrigeration::setRapidFreezing(const std::string& rapidFreezing) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(rapidFreezing);
		return executeCommand("setRapidFreezing", commandArgs);
	}

	Result<void> Refrigeration::setRapidCooling(const std::string& rapidCooling) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(rapidCooling);
		return executeCommand("setRapidCooling", commandArgs);
	}

	RelativeBrightness::RelativeBrightness(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("relativeBrightness", version, std::move(componentId), std::move(deviceId), client) {
	}

	void RelativeBrightness::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&RelativeBrightness::BrightnessIntensity>(_brightnessIntensity, static_cast<int>(attributeNumber("brightnessIntensity", _brightnessIntensity)));
	}

	RemoteControlStatus::RemoteControlStatus(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("remoteControlStatus", version, std::move(componentId), std::move(deviceId), client) {
	}

	void RemoteControlStatus::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&RemoteControlStatus::RemoteControlEnabled>(_remoteControlEnabled, attributeString("remoteControlEnabled", _remoteControlEnabled));
	}

	SoundDetection::SoundDetection(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("soundDetection", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SoundDetection::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&SoundDetection::SoundDetectionState>(_soundDetectionState, attributeString("soundDetectionState", _soundDetectionState));
		{ auto v = attributeStringArray("supportedSoundTypes"); if (!v.empty()) SetPropertyValueAndNotify<&SoundDetection::SupportedSoundTypes>(_supportedSoundTypes, v); }
		SetPropertyValueAndNotify<&SoundDetection::SoundDetected>(_soundDetected, attributeString("soundDetected", _soundDetected));
	}

	Result<void> SoundDetection::disableSoundDetection() {
		return executeCommand("disableSoundDetection");
	}

	Result<void> SoundDetection::enableSoundDetection() {
		return executeCommand("enableSoundDetection");
	}

	Switch::Switch(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("switch", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Switch::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&Switch::SwitchValue>(_switchValue, attributeString("switch", _switchValue));
	}

	Result<void> Switch::off() {
		return executeCommand("off");
	}

	Result<void> Switch::on() {
		return executeCommand("on");
	}

	SwitchLevel::SwitchLevel(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("switchLevel", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SwitchLevel::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("levelRange"); if (!v.is_null()) SetPropertyValueAndNotify<&SwitchLevel::LevelRange>(_levelRange, v); }
		SetPropertyValueAndNotify<&SwitchLevel::Level>(_level, static_cast<int>(attributeNumber("level", _level)));
	}

	Result<void> SwitchLevel::setLevel(int level, int rate) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(level);
		commandArgs.push_back(rate);
		return executeCommand("setLevel", commandArgs);
	}

	TemperatureMeasurement::TemperatureMeasurement(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("temperatureMeasurement", version, std::move(componentId), std::move(deviceId), client) {
	}

	void TemperatureMeasurement::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("temperatureRange"); if (!v.is_null()) SetPropertyValueAndNotify<&TemperatureMeasurement::TemperatureRange>(_temperatureRange, v); }
		SetPropertyValueAndNotify<&TemperatureMeasurement::Temperature>(_temperature, attributeNumber("temperature", _temperature));
	}

	ThermostatCoolingSetpoint::ThermostatCoolingSetpoint(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("thermostatCoolingSetpoint", version, std::move(componentId), std::move(deviceId), client) {
	}

	void ThermostatCoolingSetpoint::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("coolingSetpointRange"); if (!v.is_null()) SetPropertyValueAndNotify<&ThermostatCoolingSetpoint::CoolingSetpointRange>(_coolingSetpointRange, v); }
		SetPropertyValueAndNotify<&ThermostatCoolingSetpoint::CoolingSetpoint>(_coolingSetpoint, attributeNumber("coolingSetpoint", _coolingSetpoint));
	}

	Result<void> ThermostatCoolingSetpoint::setCoolingSetpoint(double setpoint) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(setpoint);
		return executeCommand("setCoolingSetpoint", commandArgs);
	}

	TvChannel::TvChannel(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("tvChannel", version, std::move(componentId), std::move(deviceId), client) {
	}

	void TvChannel::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&TvChannel::TvChannelValue>(_tvChannelValue, attributeString("tvChannel", _tvChannelValue));
		SetPropertyValueAndNotify<&TvChannel::TvChannelName>(_tvChannelName, attributeString("tvChannelName", _tvChannelName));
	}

	Result<void> TvChannel::channelDown() {
		return executeCommand("channelDown");
	}

	Result<void> TvChannel::setTvChannel(const std::string& tvChannel) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(tvChannel);
		return executeCommand("setTvChannel", commandArgs);
	}

	Result<void> TvChannel::channelUp() {
		return executeCommand("channelUp");
	}

	Result<void> TvChannel::setTvChannelName(const std::string& tvChannelName) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(tvChannelName);
		return executeCommand("setTvChannelName", commandArgs);
	}

	WasherOperatingState::WasherOperatingState(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("washerOperatingState", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherOperatingState::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WasherOperatingState::CompletionTime>(_completionTime, attributeString("completionTime", _completionTime));
		SetPropertyValueAndNotify<&WasherOperatingState::MachineState>(_machineState, attributeString("machineState", _machineState));
		SetPropertyValueAndNotify<&WasherOperatingState::WasherJobState>(_washerJobState, attributeString("washerJobState", _washerJobState));
		{ auto v = attributeStringArray("supportedMachineStates"); if (!v.empty()) SetPropertyValueAndNotify<&WasherOperatingState::SupportedMachineStates>(_supportedMachineStates, v); }
	}

	Result<void> WasherOperatingState::setMachineState(const std::string& state) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(state);
		return executeCommand("setMachineState", commandArgs);
	}

	WindowShadeLevel::WindowShadeLevel(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("windowShadeLevel", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WindowShadeLevel::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WindowShadeLevel::ShadeLevel>(_shadeLevel, static_cast<int>(attributeNumber("shadeLevel", _shadeLevel)));
	}

	Result<void> WindowShadeLevel::setShadeLevel(int shadeLevel) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(shadeLevel);
		return executeCommand("setShadeLevel", commandArgs);
	}

} // namespace standard
} // namespace smartthings4cpp
