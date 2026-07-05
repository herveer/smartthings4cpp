#include "smartthings4cpp/capability.h"
#include "smartthings4cpp/capabilities.h"

namespace smartthings4cpp {

	std::unique_ptr<Capability> createStandardCapability(const std::string& id, int version,
		const std::string& componentId, const std::string& deviceId, Client* client) {
	if (id == standard::AudioMute::CAPABILITY_ID) return std::make_unique<standard::AudioMute>(version, componentId, deviceId, client);
	if (id == standard::AudioNotification::CAPABILITY_ID) return std::make_unique<standard::AudioNotification>(version, componentId, deviceId, client);
	if (id == standard::AudioTrackData::CAPABILITY_ID) return std::make_unique<standard::AudioTrackData>(version, componentId, deviceId, client);
	if (id == standard::AudioVolume::CAPABILITY_ID) return std::make_unique<standard::AudioVolume>(version, componentId, deviceId, client);
	if (id == standard::Battery::CAPABILITY_ID) return std::make_unique<standard::Battery>(version, componentId, deviceId, client);
	if (id == standard::Bridge::CAPABILITY_ID) return std::make_unique<standard::Bridge>(version, componentId, deviceId, client);
	if (id == standard::ColorControl::CAPABILITY_ID) return std::make_unique<standard::ColorControl>(version, componentId, deviceId, client);
	if (id == standard::ColorTemperature::CAPABILITY_ID) return std::make_unique<standard::ColorTemperature>(version, componentId, deviceId, client);
	if (id == standard::ContactSensor::CAPABILITY_ID) return std::make_unique<standard::ContactSensor>(version, componentId, deviceId, client);
	if (id == standard::DemandResponseLoadControl::CAPABILITY_ID) return std::make_unique<standard::DemandResponseLoadControl>(version, componentId, deviceId, client);
	if (id == standard::DishwasherOperatingState::CAPABILITY_ID) return std::make_unique<standard::DishwasherOperatingState>(version, componentId, deviceId, client);
	if (id == standard::Execute::CAPABILITY_ID) return std::make_unique<standard::Execute>(version, componentId, deviceId, client);
	if (id == standard::HealthCheck::CAPABILITY_ID) return std::make_unique<standard::HealthCheck>(version, componentId, deviceId, client);
	if (id == standard::IlluminanceMeasurement::CAPABILITY_ID) return std::make_unique<standard::IlluminanceMeasurement>(version, componentId, deviceId, client);
	if (id == standard::MediaInputSource::CAPABILITY_ID) return std::make_unique<standard::MediaInputSource>(version, componentId, deviceId, client);
	if (id == standard::MediaPlayback::CAPABILITY_ID) return std::make_unique<standard::MediaPlayback>(version, componentId, deviceId, client);
	if (id == standard::MediaPresets::CAPABILITY_ID) return std::make_unique<standard::MediaPresets>(version, componentId, deviceId, client);
	if (id == standard::MediaTrackControl::CAPABILITY_ID) return std::make_unique<standard::MediaTrackControl>(version, componentId, deviceId, client);
	if (id == standard::MotionSensor::CAPABILITY_ID) return std::make_unique<standard::MotionSensor>(version, componentId, deviceId, client);
	if (id == standard::MovementSensor::CAPABILITY_ID) return std::make_unique<standard::MovementSensor>(version, componentId, deviceId, client);
	if (id == standard::MultipleZonePresence::CAPABILITY_ID) return std::make_unique<standard::MultipleZonePresence>(version, componentId, deviceId, client);
	if (id == standard::Ocf::CAPABILITY_ID) return std::make_unique<standard::Ocf>(version, componentId, deviceId, client);
	if (id == standard::PowerConsumptionReport::CAPABILITY_ID) return std::make_unique<standard::PowerConsumptionReport>(version, componentId, deviceId, client);
	if (id == standard::PresenceSensor::CAPABILITY_ID) return std::make_unique<standard::PresenceSensor>(version, componentId, deviceId, client);
	if (id == standard::Refresh::CAPABILITY_ID) return std::make_unique<standard::Refresh>(version, componentId, deviceId, client);
	if (id == standard::Refrigeration::CAPABILITY_ID) return std::make_unique<standard::Refrigeration>(version, componentId, deviceId, client);
	if (id == standard::RelativeBrightness::CAPABILITY_ID) return std::make_unique<standard::RelativeBrightness>(version, componentId, deviceId, client);
	if (id == standard::RemoteControlStatus::CAPABILITY_ID) return std::make_unique<standard::RemoteControlStatus>(version, componentId, deviceId, client);
	if (id == standard::SoundDetection::CAPABILITY_ID) return std::make_unique<standard::SoundDetection>(version, componentId, deviceId, client);
	if (id == standard::Switch::CAPABILITY_ID) return std::make_unique<standard::Switch>(version, componentId, deviceId, client);
	if (id == standard::SwitchLevel::CAPABILITY_ID) return std::make_unique<standard::SwitchLevel>(version, componentId, deviceId, client);
	if (id == standard::TemperatureMeasurement::CAPABILITY_ID) return std::make_unique<standard::TemperatureMeasurement>(version, componentId, deviceId, client);
	if (id == standard::ThermostatCoolingSetpoint::CAPABILITY_ID) return std::make_unique<standard::ThermostatCoolingSetpoint>(version, componentId, deviceId, client);
	if (id == standard::TvChannel::CAPABILITY_ID) return std::make_unique<standard::TvChannel>(version, componentId, deviceId, client);
	if (id == standard::WasherOperatingState::CAPABILITY_ID) return std::make_unique<standard::WasherOperatingState>(version, componentId, deviceId, client);
	if (id == standard::WindowShadeLevel::CAPABILITY_ID) return std::make_unique<standard::WindowShadeLevel>(version, componentId, deviceId, client);
	return nullptr;
	}

} // namespace smartthings4cpp
