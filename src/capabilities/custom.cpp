#include "smartthings4cpp/capabilities/custom.h"

#include <utility>

namespace smartthings4cpp {
namespace custom {

	Accessibility::Accessibility(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.accessibility", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> Accessibility::setAccessibility(const std::string& menuName, const std::string& menuId, const std::string& option) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(menuName);
		commandArgs.push_back(menuId);
		commandArgs.push_back(option);
		return executeCommand("setAccessibility", commandArgs);
	}

	DeodorFilter::DeodorFilter(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.deodorFilter", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DeodorFilter::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DeodorFilter::DeodorFilterCapacity>(_deodorFilterCapacity, static_cast<int>(attributeNumber("deodorFilterCapacity", _deodorFilterCapacity)));
		SetPropertyValueAndNotify<&DeodorFilter::DeodorFilterLastResetDate>(_deodorFilterLastResetDate, attributeString("deodorFilterLastResetDate", _deodorFilterLastResetDate));
		SetPropertyValueAndNotify<&DeodorFilter::DeodorFilterStatus>(_deodorFilterStatus, attributeString("deodorFilterStatus", _deodorFilterStatus));
		{ auto v = attributeStringArray("deodorFilterResetType"); if (!v.empty()) SetPropertyValueAndNotify<&DeodorFilter::DeodorFilterResetType>(_deodorFilterResetType, v); }
		SetPropertyValueAndNotify<&DeodorFilter::DeodorFilterUsage>(_deodorFilterUsage, static_cast<int>(attributeNumber("deodorFilterUsage", _deodorFilterUsage)));
		SetPropertyValueAndNotify<&DeodorFilter::DeodorFilterUsageStep>(_deodorFilterUsageStep, static_cast<int>(attributeNumber("deodorFilterUsageStep", _deodorFilterUsageStep)));
	}

	Result<void> DeodorFilter::resetDeodorFilter() {
		return executeCommand("resetDeodorFilter");
	}

	DeviceReportStateConfiguration::DeviceReportStateConfiguration(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.deviceReportStateConfiguration", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DeviceReportStateConfiguration::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DeviceReportStateConfiguration::ReportStateRealtimePeriod>(_reportStateRealtimePeriod, attributeString("reportStateRealtimePeriod", _reportStateRealtimePeriod));
		{ auto v = attributeValue("reportStateRealtime"); if (!v.is_null()) SetPropertyValueAndNotify<&DeviceReportStateConfiguration::ReportStateRealtime>(_reportStateRealtime, v); }
		SetPropertyValueAndNotify<&DeviceReportStateConfiguration::ReportStatePeriod>(_reportStatePeriod, attributeString("reportStatePeriod", _reportStatePeriod));
	}

	Result<void> DeviceReportStateConfiguration::setReportStatePeriod(const std::string& value) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(value);
		return executeCommand("setReportStatePeriod", commandArgs);
	}

	Result<void> DeviceReportStateConfiguration::setReportStateRealtime(const nlohmann::json& value) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(value);
		return executeCommand("setReportStateRealtime", commandArgs);
	}

	Result<void> DeviceReportStateConfiguration::setReportStateRealtimePeriod(const std::string& value) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(value);
		return executeCommand("setReportStateRealtimePeriod", commandArgs);
	}

	DisabledCapabilities::DisabledCapabilities(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.disabledCapabilities", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DisabledCapabilities::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("disabledCapabilities"); if (!v.empty()) SetPropertyValueAndNotify<&DisabledCapabilities::DisabledCapabilitiesValue>(_disabledCapabilitiesValue, v); }
	}

	DisabledComponents::DisabledComponents(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.disabledComponents", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DisabledComponents::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("disabledComponents"); if (!v.empty()) SetPropertyValueAndNotify<&DisabledComponents::DisabledComponentsValue>(_disabledComponentsValue, v); }
	}

	DishwasherDelayStartTime::DishwasherDelayStartTime(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.dishwasherDelayStartTime", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DishwasherDelayStartTime::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DishwasherDelayStartTime::DishwasherDelayStartTimeValue>(_dishwasherDelayStartTimeValue, attributeString("dishwasherDelayStartTime", _dishwasherDelayStartTimeValue));
	}

	Result<void> DishwasherDelayStartTime::setDishwasherDelayStartTime(const std::string& dishwasherDelayStartTime) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(dishwasherDelayStartTime);
		return executeCommand("setDishwasherDelayStartTime", commandArgs);
	}

	DishwasherOperatingPercentage::DishwasherOperatingPercentage(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.dishwasherOperatingPercentage", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DishwasherOperatingPercentage::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DishwasherOperatingPercentage::DishwasherOperatingPercentageValue>(_dishwasherOperatingPercentageValue, static_cast<int>(attributeNumber("dishwasherOperatingPercentage", _dishwasherOperatingPercentageValue)));
	}

	DishwasherOperatingProgress::DishwasherOperatingProgress(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.dishwasherOperatingProgress", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DishwasherOperatingProgress::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DishwasherOperatingProgress::DishwasherOperatingProgressValue>(_dishwasherOperatingProgressValue, attributeString("dishwasherOperatingProgress", _dishwasherOperatingProgressValue));
	}

	DryerDryLevel::DryerDryLevel(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.dryerDryLevel", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DryerDryLevel::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DryerDryLevel::DryerDryLevelValue>(_dryerDryLevelValue, attributeString("dryerDryLevel", _dryerDryLevelValue));
		{ auto v = attributeStringArray("supportedDryerDryLevel"); if (!v.empty()) SetPropertyValueAndNotify<&DryerDryLevel::SupportedDryerDryLevel>(_supportedDryerDryLevel, v); }
	}

	Result<void> DryerDryLevel::setDryerDryLevel(const std::string& dryLevel) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(dryLevel);
		return executeCommand("setDryerDryLevel", commandArgs);
	}

	DustFilter::DustFilter(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.dustFilter", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DustFilter::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DustFilter::DustFilterUsageStep>(_dustFilterUsageStep, static_cast<int>(attributeNumber("dustFilterUsageStep", _dustFilterUsageStep)));
		SetPropertyValueAndNotify<&DustFilter::DustFilterUsage>(_dustFilterUsage, static_cast<int>(attributeNumber("dustFilterUsage", _dustFilterUsage)));
		SetPropertyValueAndNotify<&DustFilter::DustFilterLastResetDate>(_dustFilterLastResetDate, attributeString("dustFilterLastResetDate", _dustFilterLastResetDate));
		SetPropertyValueAndNotify<&DustFilter::DustFilterStatus>(_dustFilterStatus, attributeString("dustFilterStatus", _dustFilterStatus));
		SetPropertyValueAndNotify<&DustFilter::DustFilterCapacity>(_dustFilterCapacity, static_cast<int>(attributeNumber("dustFilterCapacity", _dustFilterCapacity)));
		{ auto v = attributeStringArray("dustFilterResetType"); if (!v.empty()) SetPropertyValueAndNotify<&DustFilter::DustFilterResetType>(_dustFilterResetType, v); }
	}

	Result<void> DustFilter::resetDustFilter() {
		return executeCommand("resetDustFilter");
	}

	EnergyType::EnergyType(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.energyType", version, std::move(componentId), std::move(deviceId), client) {
	}

	void EnergyType::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&EnergyType::EnergyTypeValue>(_energyTypeValue, attributeString("energyType", _energyTypeValue));
		SetPropertyValueAndNotify<&EnergyType::EnergySavingSupport>(_energySavingSupport, attributeBool("energySavingSupport", _energySavingSupport));
		SetPropertyValueAndNotify<&EnergyType::DrMaxDuration>(_drMaxDuration, static_cast<int>(attributeNumber("drMaxDuration", _drMaxDuration)));
		SetPropertyValueAndNotify<&EnergyType::EnergySavingLevel>(_energySavingLevel, static_cast<int>(attributeNumber("energySavingLevel", _energySavingLevel)));
		{ auto v = attributeValue("energySavingInfo"); if (!v.is_null()) SetPropertyValueAndNotify<&EnergyType::EnergySavingInfo>(_energySavingInfo, v); }
		{ auto v = attributeValue("supportedEnergySavingLevels"); if (!v.is_null()) SetPropertyValueAndNotify<&EnergyType::SupportedEnergySavingLevels>(_supportedEnergySavingLevels, v); }
		SetPropertyValueAndNotify<&EnergyType::EnergySavingOperation>(_energySavingOperation, attributeBool("energySavingOperation", _energySavingOperation));
		SetPropertyValueAndNotify<&EnergyType::NotificationTemplateID>(_notificationTemplateID, attributeString("notificationTemplateID", _notificationTemplateID));
		SetPropertyValueAndNotify<&EnergyType::EnergySavingOperationSupport>(_energySavingOperationSupport, attributeBool("energySavingOperationSupport", _energySavingOperationSupport));
	}

	Result<void> EnergyType::setEnergySavingLevel(int energySavingLevel) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(energySavingLevel);
		return executeCommand("setEnergySavingLevel", commandArgs);
	}

	Error::Error(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.error", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Error::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("error"); if (!v.is_null()) SetPropertyValueAndNotify<&Error::ErrorValue>(_errorValue, v); }
	}

	Result<void> Error::setError(const nlohmann::json& error) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(error);
		return executeCommand("setError", commandArgs);
	}

	FridgeMode::FridgeMode(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.fridgeMode", version, std::move(componentId), std::move(deviceId), client) {
	}

	void FridgeMode::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&FridgeMode::FridgeModeValue>(_fridgeModeValue, attributeString("fridgeModeValue", _fridgeModeValue));
		SetPropertyValueAndNotify<&FridgeMode::FridgeModeValue_>(_fridgeModeValue_, attributeString("fridgeMode", _fridgeModeValue_));
		{ auto v = attributeStringArray("supportedFridgeOptions"); if (!v.empty()) SetPropertyValueAndNotify<&FridgeMode::SupportedFridgeOptions>(_supportedFridgeOptions, v); }
		{ auto v = attributeStringArray("supportedFullFridgeModes"); if (!v.empty()) SetPropertyValueAndNotify<&FridgeMode::SupportedFullFridgeModes>(_supportedFullFridgeModes, v); }
		{ auto v = attributeStringArray("supportedFridgeModes"); if (!v.empty()) SetPropertyValueAndNotify<&FridgeMode::SupportedFridgeModes>(_supportedFridgeModes, v); }
	}

	Result<void> FridgeMode::setFridgeMode(const std::string& fridgeMode) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(fridgeMode);
		return executeCommand("setFridgeMode", commandArgs);
	}

	JobBeginningStatus::JobBeginningStatus(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.jobBeginningStatus", version, std::move(componentId), std::move(deviceId), client) {
	}

	void JobBeginningStatus::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&JobBeginningStatus::JobBeginningStatusValue>(_jobBeginningStatusValue, attributeString("jobBeginningStatus", _jobBeginningStatusValue));
	}

	Launchapp::Launchapp(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.launchapp", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> Launchapp::launchApp(const std::string& appId, const std::string& appName) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(appId);
		commandArgs.push_back(appName);
		return executeCommand("launchApp", commandArgs);
	}

	Picturemode::Picturemode(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.picturemode", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Picturemode::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&Picturemode::PictureModeValue>(_pictureModeValue, attributeString("pictureMode", _pictureModeValue));
		{ auto v = attributeValue("supportedPictureModes"); if (!v.is_null()) SetPropertyValueAndNotify<&Picturemode::SupportedPictureModes>(_supportedPictureModes, v); }
		{ auto v = attributeValue("supportedPictureModesMap"); if (!v.is_null()) SetPropertyValueAndNotify<&Picturemode::SupportedPictureModesMap>(_supportedPictureModesMap, v); }
	}

	Result<void> Picturemode::setPictureMode(const std::string& mode) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(mode);
		return executeCommand("setPictureMode", commandArgs);
	}

	Recording::Recording(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.recording", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> Recording::recordStart() {
		return executeCommand("recordStart");
	}

	Result<void> Recording::recordStop() {
		return executeCommand("recordStop");
	}

	Soundmode::Soundmode(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.soundmode", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Soundmode::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("supportedSoundModesMap"); if (!v.is_null()) SetPropertyValueAndNotify<&Soundmode::SupportedSoundModesMap>(_supportedSoundModesMap, v); }
		SetPropertyValueAndNotify<&Soundmode::SoundModeValue>(_soundModeValue, attributeString("soundMode", _soundModeValue));
		{ auto v = attributeStringArray("supportedSoundModes"); if (!v.empty()) SetPropertyValueAndNotify<&Soundmode::SupportedSoundModes>(_supportedSoundModes, v); }
	}

	Result<void> Soundmode::setSoundMode(const std::string& mode) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(mode);
		return executeCommand("setSoundMode", commandArgs);
	}

	SupportedOptions::SupportedOptions(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.supportedOptions", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SupportedOptions::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&SupportedOptions::Course>(_course, attributeString("course", _course));
		{ auto v = attributeValue("referenceTable"); if (!v.is_null()) SetPropertyValueAndNotify<&SupportedOptions::ReferenceTable>(_referenceTable, v); }
		{ auto v = attributeStringArray("supportedCourses"); if (!v.empty()) SetPropertyValueAndNotify<&SupportedOptions::SupportedCourses>(_supportedCourses, v); }
	}

	Result<void> SupportedOptions::setCourse(const std::string& course) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(course);
		return executeCommand("setCourse", commandArgs);
	}

	ThermostatSetpointControl::ThermostatSetpointControl(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.thermostatSetpointControl", version, std::move(componentId), std::move(deviceId), client) {
	}

	void ThermostatSetpointControl::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&ThermostatSetpointControl::MinimumSetpoint>(_minimumSetpoint, attributeNumber("minimumSetpoint", _minimumSetpoint));
		SetPropertyValueAndNotify<&ThermostatSetpointControl::MaximumSetpoint>(_maximumSetpoint, attributeNumber("maximumSetpoint", _maximumSetpoint));
	}

	Result<void> ThermostatSetpointControl::raiseSetpoint() {
		return executeCommand("raiseSetpoint");
	}

	Result<void> ThermostatSetpointControl::lowerSetpoint() {
		return executeCommand("lowerSetpoint");
	}

	Tvsearch::Tvsearch(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.tvsearch", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> Tvsearch::search(const std::string& query, const std::string& url) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(query);
		commandArgs.push_back(url);
		return executeCommand("search", commandArgs);
	}

	WasherAutoDetergent::WasherAutoDetergent(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.washerAutoDetergent", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherAutoDetergent::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WasherAutoDetergent::WasherAutoDetergentValue>(_washerAutoDetergentValue, attributeString("washerAutoDetergent", _washerAutoDetergentValue));
	}

	Result<void> WasherAutoDetergent::setWasherAutoDetergent(const std::string& washerAutoDetergent) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(washerAutoDetergent);
		return executeCommand("setWasherAutoDetergent", commandArgs);
	}

	WasherAutoSoftener::WasherAutoSoftener(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.washerAutoSoftener", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherAutoSoftener::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WasherAutoSoftener::WasherAutoSoftenerValue>(_washerAutoSoftenerValue, attributeString("washerAutoSoftener", _washerAutoSoftenerValue));
	}

	Result<void> WasherAutoSoftener::setWasherAutoSoftener(const std::string& washerAutoSoftener) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(washerAutoSoftener);
		return executeCommand("setWasherAutoSoftener", commandArgs);
	}

	WasherRinseCycles::WasherRinseCycles(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.washerRinseCycles", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherRinseCycles::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedWasherRinseCycles"); if (!v.empty()) SetPropertyValueAndNotify<&WasherRinseCycles::SupportedWasherRinseCycles>(_supportedWasherRinseCycles, v); }
		SetPropertyValueAndNotify<&WasherRinseCycles::WasherRinseCyclesValue>(_washerRinseCyclesValue, attributeString("washerRinseCycles", _washerRinseCyclesValue));
	}

	Result<void> WasherRinseCycles::setWasherRinseCycles(const std::string& cycle) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(cycle);
		return executeCommand("setWasherRinseCycles", commandArgs);
	}

	WasherSoilLevel::WasherSoilLevel(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.washerSoilLevel", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherSoilLevel::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedWasherSoilLevel"); if (!v.empty()) SetPropertyValueAndNotify<&WasherSoilLevel::SupportedWasherSoilLevel>(_supportedWasherSoilLevel, v); }
		SetPropertyValueAndNotify<&WasherSoilLevel::WasherSoilLevelValue>(_washerSoilLevelValue, attributeString("washerSoilLevel", _washerSoilLevelValue));
	}

	Result<void> WasherSoilLevel::setWasherSoilLevel(const std::string& soilLevel) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(soilLevel);
		return executeCommand("setWasherSoilLevel", commandArgs);
	}

	WasherSpinLevel::WasherSpinLevel(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.washerSpinLevel", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherSpinLevel::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WasherSpinLevel::WasherSpinLevelValue>(_washerSpinLevelValue, attributeString("washerSpinLevel", _washerSpinLevelValue));
		{ auto v = attributeStringArray("supportedWasherSpinLevel"); if (!v.empty()) SetPropertyValueAndNotify<&WasherSpinLevel::SupportedWasherSpinLevel>(_supportedWasherSpinLevel, v); }
	}

	Result<void> WasherSpinLevel::setWasherSpinLevel(const std::string& spinLevel) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(spinLevel);
		return executeCommand("setWasherSpinLevel", commandArgs);
	}

	WasherWaterTemperature::WasherWaterTemperature(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.washerWaterTemperature", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherWaterTemperature::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedWasherWaterTemperature"); if (!v.empty()) SetPropertyValueAndNotify<&WasherWaterTemperature::SupportedWasherWaterTemperature>(_supportedWasherWaterTemperature, v); }
		SetPropertyValueAndNotify<&WasherWaterTemperature::WasherWaterTemperatureValue>(_washerWaterTemperatureValue, attributeString("washerWaterTemperature", _washerWaterTemperatureValue));
	}

	Result<void> WasherWaterTemperature::setWasherWaterTemperature(const std::string& temperature) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(temperature);
		return executeCommand("setWasherWaterTemperature", commandArgs);
	}

	WaterFilter::WaterFilter(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("custom.waterFilter", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WaterFilter::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WaterFilter::WaterFilterUsageStep>(_waterFilterUsageStep, static_cast<int>(attributeNumber("waterFilterUsageStep", _waterFilterUsageStep)));
		{ auto v = attributeStringArray("waterFilterResetType"); if (!v.empty()) SetPropertyValueAndNotify<&WaterFilter::WaterFilterResetType>(_waterFilterResetType, v); }
		SetPropertyValueAndNotify<&WaterFilter::WaterFilterCapacity>(_waterFilterCapacity, static_cast<int>(attributeNumber("waterFilterCapacity", _waterFilterCapacity)));
		SetPropertyValueAndNotify<&WaterFilter::WaterFilterLastResetDate>(_waterFilterLastResetDate, attributeString("waterFilterLastResetDate", _waterFilterLastResetDate));
		SetPropertyValueAndNotify<&WaterFilter::WaterFilterUsage>(_waterFilterUsage, static_cast<int>(attributeNumber("waterFilterUsage", _waterFilterUsage)));
		SetPropertyValueAndNotify<&WaterFilter::WaterFilterStatus>(_waterFilterStatus, attributeString("waterFilterStatus", _waterFilterStatus));
	}

	Result<void> WaterFilter::resetWaterFilter() {
		return executeCommand("resetWaterFilter");
	}

} // namespace custom
} // namespace smartthings4cpp
