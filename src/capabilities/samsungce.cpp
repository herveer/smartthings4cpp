#include "smartthings4cpp/capabilities/samsungce.h"

#include <utility>

namespace smartthings4cpp {
namespace samsungce {

	Accessibility::Accessibility(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.accessibility", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Accessibility::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedColorInversions"); if (!v.empty()) SetPropertyValueAndNotify<&Accessibility::SupportedColorInversions>(_supportedColorInversions, v); }
		SetPropertyValueAndNotify<&Accessibility::Magnification>(_magnification, attributeString("magnification", _magnification));
		SetPropertyValueAndNotify<&Accessibility::MagnificationLevel>(_magnificationLevel, static_cast<int>(attributeNumber("magnificationLevel", _magnificationLevel)));
		SetPropertyValueAndNotify<&Accessibility::HighContrast>(_highContrast, attributeString("highContrast", _highContrast));
		SetPropertyValueAndNotify<&Accessibility::VoiceGuide>(_voiceGuide, attributeString("voiceGuide", _voiceGuide));
		{ auto v = attributeStringArray("supportedVoiceGuides"); if (!v.empty()) SetPropertyValueAndNotify<&Accessibility::SupportedVoiceGuides>(_supportedVoiceGuides, v); }
		SetPropertyValueAndNotify<&Accessibility::ColorInversion>(_colorInversion, attributeString("colorInversion", _colorInversion));
		SetPropertyValueAndNotify<&Accessibility::Grayscale>(_grayscale, attributeString("grayscale", _grayscale));
		{ auto v = attributeStringArray("supportedReluminos"); if (!v.empty()) SetPropertyValueAndNotify<&Accessibility::SupportedReluminos>(_supportedReluminos, v); }
		{ auto v = attributeValue("supportedMagnificationLevels"); if (!v.is_null()) SetPropertyValueAndNotify<&Accessibility::SupportedMagnificationLevels>(_supportedMagnificationLevels, v); }
		{ auto v = attributeStringArray("supportedMagnifications"); if (!v.empty()) SetPropertyValueAndNotify<&Accessibility::SupportedMagnifications>(_supportedMagnifications, v); }
		SetPropertyValueAndNotify<&Accessibility::Relumino>(_relumino, attributeString("relumino", _relumino));
		{ auto v = attributeStringArray("supportedOptions"); if (!v.empty()) SetPropertyValueAndNotify<&Accessibility::SupportedOptions>(_supportedOptions, v); }
		{ auto v = attributeStringArray("supportedHighContrasts"); if (!v.empty()) SetPropertyValueAndNotify<&Accessibility::SupportedHighContrasts>(_supportedHighContrasts, v); }
		{ auto v = attributeValue("supportedFontSizes"); if (!v.is_null()) SetPropertyValueAndNotify<&Accessibility::SupportedFontSizes>(_supportedFontSizes, v); }
		SetPropertyValueAndNotify<&Accessibility::FontSize>(_fontSize, static_cast<int>(attributeNumber("fontSize", _fontSize)));
		{ auto v = attributeStringArray("supportedGrayscales"); if (!v.empty()) SetPropertyValueAndNotify<&Accessibility::SupportedGrayscales>(_supportedGrayscales, v); }
	}

	Result<void> Accessibility::setVoiceGuide(const std::string& voiceGuide) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(voiceGuide);
		return executeCommand("setVoiceGuide", commandArgs);
	}

	Result<void> Accessibility::setRelumino(const std::string& relumino) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(relumino);
		return executeCommand("setRelumino", commandArgs);
	}

	Result<void> Accessibility::setMagnification(const std::string& magnification) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(magnification);
		return executeCommand("setMagnification", commandArgs);
	}

	Result<void> Accessibility::setHighContrast(const std::string& highContrast) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(highContrast);
		return executeCommand("setHighContrast", commandArgs);
	}

	Result<void> Accessibility::setGrayscale(const std::string& grayscale) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(grayscale);
		return executeCommand("setGrayscale", commandArgs);
	}

	Result<void> Accessibility::setColorInversion(const std::string& colorInversion) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(colorInversion);
		return executeCommand("setColorInversion", commandArgs);
	}

	Result<void> Accessibility::setMagnificationLevel(int magnificationLevel) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(magnificationLevel);
		return executeCommand("setMagnificationLevel", commandArgs);
	}

	Result<void> Accessibility::setFontSize(int fontSize) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(fontSize);
		return executeCommand("setFontSize", commandArgs);
	}

	AudioVolumeLevel::AudioVolumeLevel(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.audioVolumeLevel", version, std::move(componentId), std::move(deviceId), client) {
	}

	void AudioVolumeLevel::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&AudioVolumeLevel::VolumeLevel>(_volumeLevel, static_cast<int>(attributeNumber("volumeLevel", _volumeLevel)));
		{ auto v = attributeValue("volumeLevelRange"); if (!v.is_null()) SetPropertyValueAndNotify<&AudioVolumeLevel::VolumeLevelRange>(_volumeLevelRange, v); }
	}

	Result<void> AudioVolumeLevel::volumeDown() {
		return executeCommand("volumeDown");
	}

	Result<void> AudioVolumeLevel::volumeUp() {
		return executeCommand("volumeUp");
	}

	Result<void> AudioVolumeLevel::setVolumeLevel(int volumeLevel) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(volumeLevel);
		return executeCommand("setVolumeLevel", commandArgs);
	}

	AutoDispenseDetergent::AutoDispenseDetergent(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.autoDispenseDetergent", version, std::move(componentId), std::move(deviceId), client) {
	}

	void AutoDispenseDetergent::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&AutoDispenseDetergent::RemainingAmount>(_remainingAmount, attributeString("remainingAmount", _remainingAmount));
		SetPropertyValueAndNotify<&AutoDispenseDetergent::Amount>(_amount, attributeString("amount", _amount));
		{ auto v = attributeStringArray("supportedDensity"); if (!v.empty()) SetPropertyValueAndNotify<&AutoDispenseDetergent::SupportedDensity>(_supportedDensity, v); }
		SetPropertyValueAndNotify<&AutoDispenseDetergent::Density>(_density, attributeString("density", _density));
		{ auto v = attributeStringArray("supportedAmount"); if (!v.empty()) SetPropertyValueAndNotify<&AutoDispenseDetergent::SupportedAmount>(_supportedAmount, v); }
		{ auto v = attributeStringArray("availableTypes"); if (!v.empty()) SetPropertyValueAndNotify<&AutoDispenseDetergent::AvailableTypes>(_availableTypes, v); }
		SetPropertyValueAndNotify<&AutoDispenseDetergent::Type>(_type, attributeString("type", _type));
		{ auto v = attributeStringArray("supportedTypes"); if (!v.empty()) SetPropertyValueAndNotify<&AutoDispenseDetergent::SupportedTypes>(_supportedTypes, v); }
		SetPropertyValueAndNotify<&AutoDispenseDetergent::RecommendedAmount>(_recommendedAmount, static_cast<int>(attributeNumber("recommendedAmount", _recommendedAmount)));
	}

	Result<void> AutoDispenseDetergent::unsetRecommendedAmount() {
		return executeCommand("unsetRecommendedAmount");
	}

	Result<void> AutoDispenseDetergent::setRecommendedAmount(const std::string& type, int amountPerDose) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(type);
		commandArgs.push_back(amountPerDose);
		return executeCommand("setRecommendedAmount", commandArgs);
	}

	Result<void> AutoDispenseDetergent::setType(const std::string& type) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(type);
		return executeCommand("setType", commandArgs);
	}

	Result<void> AutoDispenseDetergent::setAmount(const std::string& amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(amount);
		return executeCommand("setAmount", commandArgs);
	}

	Result<void> AutoDispenseDetergent::setDensity(const std::string& density) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(density);
		return executeCommand("setDensity", commandArgs);
	}

	AutoDispenseSoftener::AutoDispenseSoftener(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.autoDispenseSoftener", version, std::move(componentId), std::move(deviceId), client) {
	}

	void AutoDispenseSoftener::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&AutoDispenseSoftener::RemainingAmount>(_remainingAmount, attributeString("remainingAmount", _remainingAmount));
		SetPropertyValueAndNotify<&AutoDispenseSoftener::Amount>(_amount, attributeString("amount", _amount));
		{ auto v = attributeStringArray("supportedDensity"); if (!v.empty()) SetPropertyValueAndNotify<&AutoDispenseSoftener::SupportedDensity>(_supportedDensity, v); }
		SetPropertyValueAndNotify<&AutoDispenseSoftener::Density>(_density, attributeString("density", _density));
		{ auto v = attributeStringArray("supportedAmount"); if (!v.empty()) SetPropertyValueAndNotify<&AutoDispenseSoftener::SupportedAmount>(_supportedAmount, v); }
	}

	Result<void> AutoDispenseSoftener::setAmount(const std::string& amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(amount);
		return executeCommand("setAmount", commandArgs);
	}

	Result<void> AutoDispenseSoftener::setDensity(const std::string& density) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(density);
		return executeCommand("setDensity", commandArgs);
	}

	AutoOpenDoor::AutoOpenDoor(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.autoOpenDoor", version, std::move(componentId), std::move(deviceId), client) {
	}

	void AutoOpenDoor::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&AutoOpenDoor::AutoOpenDoorValue>(_autoOpenDoorValue, attributeString("autoOpenDoor", _autoOpenDoorValue));
		{ auto v = attributeStringArray("supportedPressureLevels"); if (!v.empty()) SetPropertyValueAndNotify<&AutoOpenDoor::SupportedPressureLevels>(_supportedPressureLevels, v); }
		SetPropertyValueAndNotify<&AutoOpenDoor::PressureLevel>(_pressureLevel, attributeString("pressureLevel", _pressureLevel));
	}

	Result<void> AutoOpenDoor::off() {
		return executeCommand("off");
	}

	Result<void> AutoOpenDoor::on() {
		return executeCommand("on");
	}

	Result<void> AutoOpenDoor::setPressureLevel(const std::string& pressureLevel) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(pressureLevel);
		return executeCommand("setPressureLevel", commandArgs);
	}

	ConnectionState::ConnectionState(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.connectionState", version, std::move(componentId), std::move(deviceId), client) {
	}

	void ConnectionState::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&ConnectionState::ConnectionStateValue>(_connectionStateValue, attributeString("connectionState", _connectionStateValue));
	}

	DetergentAutoReplenishment::DetergentAutoReplenishment(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.detergentAutoReplenishment", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DetergentAutoReplenishment::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::NeutralDetergentType>(_neutralDetergentType, attributeString("neutralDetergentType", _neutralDetergentType));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::RegularDetergentRemainingAmount>(_regularDetergentRemainingAmount, attributeNumber("regularDetergentRemainingAmount", _regularDetergentRemainingAmount));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::BabyDetergentRemainingAmount>(_babyDetergentRemainingAmount, attributeNumber("babyDetergentRemainingAmount", _babyDetergentRemainingAmount));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::NeutralDetergentRemainingAmount>(_neutralDetergentRemainingAmount, attributeNumber("neutralDetergentRemainingAmount", _neutralDetergentRemainingAmount));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::NeutralDetergentAlarmEnabled>(_neutralDetergentAlarmEnabled, attributeBool("neutralDetergentAlarmEnabled", _neutralDetergentAlarmEnabled));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::NeutralDetergentOrderThreshold>(_neutralDetergentOrderThreshold, attributeNumber("neutralDetergentOrderThreshold", _neutralDetergentOrderThreshold));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::BabyDetergentInitialAmount>(_babyDetergentInitialAmount, attributeNumber("babyDetergentInitialAmount", _babyDetergentInitialAmount));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::BabyDetergentType>(_babyDetergentType, attributeString("babyDetergentType", _babyDetergentType));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::NeutralDetergentInitialAmount>(_neutralDetergentInitialAmount, attributeNumber("neutralDetergentInitialAmount", _neutralDetergentInitialAmount));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::RegularDetergentDosage>(_regularDetergentDosage, attributeNumber("regularDetergentDosage", _regularDetergentDosage));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::BabyDetergentDosage>(_babyDetergentDosage, attributeNumber("babyDetergentDosage", _babyDetergentDosage));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::RegularDetergentOrderThreshold>(_regularDetergentOrderThreshold, attributeNumber("regularDetergentOrderThreshold", _regularDetergentOrderThreshold));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::RegularDetergentType>(_regularDetergentType, attributeString("regularDetergentType", _regularDetergentType));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::RegularDetergentInitialAmount>(_regularDetergentInitialAmount, attributeNumber("regularDetergentInitialAmount", _regularDetergentInitialAmount));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::RegularDetergentAlarmEnabled>(_regularDetergentAlarmEnabled, attributeBool("regularDetergentAlarmEnabled", _regularDetergentAlarmEnabled));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::NeutralDetergentDosage>(_neutralDetergentDosage, attributeNumber("neutralDetergentDosage", _neutralDetergentDosage));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::BabyDetergentOrderThreshold>(_babyDetergentOrderThreshold, attributeNumber("babyDetergentOrderThreshold", _babyDetergentOrderThreshold));
		SetPropertyValueAndNotify<&DetergentAutoReplenishment::BabyDetergentAlarmEnabled>(_babyDetergentAlarmEnabled, attributeBool("babyDetergentAlarmEnabled", _babyDetergentAlarmEnabled));
	}

	Result<void> DetergentAutoReplenishment::enableAlarm(const std::string& name) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		return executeCommand("enableAlarm", commandArgs);
	}

	Result<void> DetergentAutoReplenishment::setOrderThreshold(const std::string& name, double threshold) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		commandArgs.push_back(threshold);
		return executeCommand("setOrderThreshold", commandArgs);
	}

	Result<void> DetergentAutoReplenishment::disableAlarm(const std::string& name) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		return executeCommand("disableAlarm", commandArgs);
	}

	Result<void> DetergentAutoReplenishment::setAutoReplenishment(const nlohmann::json& autoReplenishment) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(autoReplenishment);
		return executeCommand("setAutoReplenishment", commandArgs);
	}

	Result<void> DetergentAutoReplenishment::setInitialAmount(const std::string& name, double amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		commandArgs.push_back(amount);
		return executeCommand("setInitialAmount", commandArgs);
	}

	Result<void> DetergentAutoReplenishment::setRemainingAmount(const std::string& name, double amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		commandArgs.push_back(amount);
		return executeCommand("setRemainingAmount", commandArgs);
	}

	Result<void> DetergentAutoReplenishment::setDosage(const std::string& name, double amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		commandArgs.push_back(amount);
		return executeCommand("setDosage", commandArgs);
	}

	Result<void> DetergentAutoReplenishment::setType(const std::string& name, const std::string& type) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		commandArgs.push_back(type);
		return executeCommand("setType", commandArgs);
	}

	DetergentOrder::DetergentOrder(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.detergentOrder", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DetergentOrder::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DetergentOrder::AlarmEnabled>(_alarmEnabled, attributeBool("alarmEnabled", _alarmEnabled));
		SetPropertyValueAndNotify<&DetergentOrder::OrderThreshold>(_orderThreshold, attributeNumber("orderThreshold", _orderThreshold));
	}

	Result<void> DetergentOrder::enableAlarm() {
		return executeCommand("enableAlarm");
	}

	Result<void> DetergentOrder::setOrderThreshold(double threshold) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(threshold);
		return executeCommand("setOrderThreshold", commandArgs);
	}

	Result<void> DetergentOrder::disableAlarm() {
		return executeCommand("disableAlarm");
	}

	DetergentState::DetergentState(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.detergentState", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DetergentState::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DetergentState::RemainingAmount>(_remainingAmount, attributeNumber("remainingAmount", _remainingAmount));
		SetPropertyValueAndNotify<&DetergentState::Dosage>(_dosage, attributeNumber("dosage", _dosage));
		SetPropertyValueAndNotify<&DetergentState::InitialAmount>(_initialAmount, attributeNumber("initialAmount", _initialAmount));
		SetPropertyValueAndNotify<&DetergentState::DetergentType>(_detergentType, attributeString("detergentType", _detergentType));
	}

	Result<void> DetergentState::setDetergentType(const std::string& detergentType) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(detergentType);
		return executeCommand("setDetergentType", commandArgs);
	}

	Result<void> DetergentState::setInitialAmount(double amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(amount);
		return executeCommand("setInitialAmount", commandArgs);
	}

	Result<void> DetergentState::setRemainingAmount(double amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(amount);
		return executeCommand("setRemainingAmount", commandArgs);
	}

	Result<void> DetergentState::setDosage(double amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(amount);
		return executeCommand("setDosage", commandArgs);
	}

	DeviceApplicationsAndSettings::DeviceApplicationsAndSettings(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.deviceApplicationsAndSettings", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DeviceApplicationsAndSettings::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("deviceApplications"); if (!v.is_null()) SetPropertyValueAndNotify<&DeviceApplicationsAndSettings::DeviceApplications>(_deviceApplications, v); }
		{ auto v = attributeValue("deviceSettings"); if (!v.is_null()) SetPropertyValueAndNotify<&DeviceApplicationsAndSettings::DeviceSettings>(_deviceSettings, v); }
	}

	DeviceIdentification::DeviceIdentification(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.deviceIdentification", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DeviceIdentification::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DeviceIdentification::MicomAssayCode>(_micomAssayCode, attributeString("micomAssayCode", _micomAssayCode));
		SetPropertyValueAndNotify<&DeviceIdentification::ModelName>(_modelName, attributeString("modelName", _modelName));
		SetPropertyValueAndNotify<&DeviceIdentification::SerialNumber>(_serialNumber, attributeString("serialNumber", _serialNumber));
		SetPropertyValueAndNotify<&DeviceIdentification::SerialNumberExtra>(_serialNumberExtra, attributeString("serialNumberExtra", _serialNumberExtra));
		SetPropertyValueAndNotify<&DeviceIdentification::ReleaseCountry>(_releaseCountry, attributeString("releaseCountry", _releaseCountry));
		SetPropertyValueAndNotify<&DeviceIdentification::ModelClassificationCode>(_modelClassificationCode, attributeString("modelClassificationCode", _modelClassificationCode));
		SetPropertyValueAndNotify<&DeviceIdentification::Description>(_description, attributeString("description", _description));
		SetPropertyValueAndNotify<&DeviceIdentification::ReleaseYear>(_releaseYear, static_cast<int>(attributeNumber("releaseYear", _releaseYear)));
		SetPropertyValueAndNotify<&DeviceIdentification::BinaryId>(_binaryId, attributeString("binaryId", _binaryId));
	}

	DishwasherJobState::DishwasherJobState(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.dishwasherJobState", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DishwasherJobState::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("scheduledJobs"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherJobState::ScheduledJobs>(_scheduledJobs, v); }
		SetPropertyValueAndNotify<&DishwasherJobState::DishwasherJobStateValue>(_dishwasherJobStateValue, attributeString("dishwasherJobState", _dishwasherJobStateValue));
	}

	DishwasherOperation::DishwasherOperation(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.dishwasherOperation", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DishwasherOperation::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedOperatingState"); if (!v.empty()) SetPropertyValueAndNotify<&DishwasherOperation::SupportedOperatingState>(_supportedOperatingState, v); }
		SetPropertyValueAndNotify<&DishwasherOperation::OperatingState>(_operatingState, attributeString("operatingState", _operatingState));
		SetPropertyValueAndNotify<&DishwasherOperation::Reservable>(_reservable, attributeBool("reservable", _reservable));
		SetPropertyValueAndNotify<&DishwasherOperation::ProgressPercentage>(_progressPercentage, attributeNumber("progressPercentage", _progressPercentage));
		SetPropertyValueAndNotify<&DishwasherOperation::RemainingTimeStr>(_remainingTimeStr, attributeString("remainingTimeStr", _remainingTimeStr));
		SetPropertyValueAndNotify<&DishwasherOperation::OperationTime>(_operationTime, attributeNumber("operationTime", _operationTime));
		SetPropertyValueAndNotify<&DishwasherOperation::RemainingTime>(_remainingTime, attributeNumber("remainingTime", _remainingTime));
		SetPropertyValueAndNotify<&DishwasherOperation::TimeLeftToStart>(_timeLeftToStart, attributeNumber("timeLeftToStart", _timeLeftToStart));
	}

	Result<void> DishwasherOperation::resume() {
		return executeCommand("resume");
	}

	Result<void> DishwasherOperation::cancel(bool drain) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(drain);
		return executeCommand("cancel", commandArgs);
	}

	Result<void> DishwasherOperation::setOperatingState(const std::string& operatingState) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(operatingState);
		return executeCommand("setOperatingState", commandArgs);
	}

	Result<void> DishwasherOperation::start(const nlohmann::json& option) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(option);
		return executeCommand("start", commandArgs);
	}

	Result<void> DishwasherOperation::startLater(double delay) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(delay);
		return executeCommand("startLater", commandArgs);
	}

	Result<void> DishwasherOperation::pause() {
		return executeCommand("pause");
	}

	DishwasherWashingCourse::DishwasherWashingCourse(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.dishwasherWashingCourse", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DishwasherWashingCourse::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("customCourseCandidates"); if (!v.empty()) SetPropertyValueAndNotify<&DishwasherWashingCourse::CustomCourseCandidates>(_customCourseCandidates, v); }
		SetPropertyValueAndNotify<&DishwasherWashingCourse::WashingCourse>(_washingCourse, attributeString("washingCourse", _washingCourse));
		{ auto v = attributeStringArray("supportedCourses"); if (!v.empty()) SetPropertyValueAndNotify<&DishwasherWashingCourse::SupportedCourses>(_supportedCourses, v); }
	}

	Result<void> DishwasherWashingCourse::setWashingCourse(const std::string& course) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(course);
		return executeCommand("setWashingCourse", commandArgs);
	}

	Result<void> DishwasherWashingCourse::setCustomCourse(const std::string& course) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(course);
		return executeCommand("setCustomCourse", commandArgs);
	}

	Result<void> DishwasherWashingCourse::startWashingCourse(const std::string& course) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(course);
		return executeCommand("startWashingCourse", commandArgs);
	}

	Result<void> DishwasherWashingCourse::startWashingCourseWithOptions(const std::string& course, const nlohmann::json& options) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(course);
		commandArgs.push_back(options);
		return executeCommand("startWashingCourseWithOptions", commandArgs);
	}

	DishwasherWashingCourseDetails::DishwasherWashingCourseDetails(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.dishwasherWashingCourseDetails", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DishwasherWashingCourseDetails::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("predefinedCourses"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingCourseDetails::PredefinedCourses>(_predefinedCourses, v); }
		SetPropertyValueAndNotify<&DishwasherWashingCourseDetails::WaterUsageMax>(_waterUsageMax, static_cast<int>(attributeNumber("waterUsageMax", _waterUsageMax)));
		SetPropertyValueAndNotify<&DishwasherWashingCourseDetails::EnergyUsageMax>(_energyUsageMax, static_cast<int>(attributeNumber("energyUsageMax", _energyUsageMax)));
	}

	DishwasherWashingOptions::DishwasherWashingOptions(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.dishwasherWashingOptions", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DishwasherWashingOptions::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("dryPlus"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::DryPlus>(_dryPlus, v); }
		{ auto v = attributeValue("stormWash"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::StormWash>(_stormWash, v); }
		{ auto v = attributeValue("multiTab"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::MultiTab>(_multiTab, v); }
		{ auto v = attributeValue("hotAirDry"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::HotAirDry>(_hotAirDry, v); }
		{ auto v = attributeValue("selectedZone"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::SelectedZone>(_selectedZone, v); }
		{ auto v = attributeValue("speedBooster"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::SpeedBooster>(_speedBooster, v); }
		{ auto v = attributeValue("highTempWash"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::HighTempWash>(_highTempWash, v); }
		{ auto v = attributeValue("sanitizingWash"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::SanitizingWash>(_sanitizingWash, v); }
		{ auto v = attributeValue("heatedDry"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::HeatedDry>(_heatedDry, v); }
		{ auto v = attributeValue("zoneBooster"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::ZoneBooster>(_zoneBooster, v); }
		{ auto v = attributeValue("addRinse"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::AddRinse>(_addRinse, v); }
		{ auto v = attributeStringArray("supportedList"); if (!v.empty()) SetPropertyValueAndNotify<&DishwasherWashingOptions::SupportedList>(_supportedList, v); }
		{ auto v = attributeValue("rinsePlus"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::RinsePlus>(_rinsePlus, v); }
		{ auto v = attributeValue("sanitize"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::Sanitize>(_sanitize, v); }
		{ auto v = attributeValue("steamSoak"); if (!v.is_null()) SetPropertyValueAndNotify<&DishwasherWashingOptions::SteamSoak>(_steamSoak, v); }
	}

	Result<void> DishwasherWashingOptions::setSelectedZone(const std::string& selectedZone) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(selectedZone);
		return executeCommand("setSelectedZone", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setSanitize(bool sanitize) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(sanitize);
		return executeCommand("setSanitize", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setZoneBooster(const std::string& zoneBooster) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(zoneBooster);
		return executeCommand("setZoneBooster", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setDryPlus(bool dryPlus) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(dryPlus);
		return executeCommand("setDryPlus", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setAddRinse(bool addRinse) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(addRinse);
		return executeCommand("setAddRinse", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setHotAirDry(bool hotAirDry) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(hotAirDry);
		return executeCommand("setHotAirDry", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setStormWash(bool stormWash) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(stormWash);
		return executeCommand("setStormWash", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setHeatedDry(const std::string& heatedDry) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(heatedDry);
		return executeCommand("setHeatedDry", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setHighTempWash(bool highTempWash) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(highTempWash);
		return executeCommand("setHighTempWash", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setSanitizingWash(bool sanitizingWash) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(sanitizingWash);
		return executeCommand("setSanitizingWash", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setMultiTab(bool multiTab) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(multiTab);
		return executeCommand("setMultiTab", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setSpeedBooster(bool speedBooster) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(speedBooster);
		return executeCommand("setSpeedBooster", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setRinsePlus(bool rinsePlus) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(rinsePlus);
		return executeCommand("setRinsePlus", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setOptions(const nlohmann::json& options) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(options);
		return executeCommand("setOptions", commandArgs);
	}

	Result<void> DishwasherWashingOptions::setSteamSoak(bool steamSoak) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(steamSoak);
		return executeCommand("setSteamSoak", commandArgs);
	}

	DongleSoftwareInstallation::DongleSoftwareInstallation(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.dongleSoftwareInstallation", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DongleSoftwareInstallation::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DongleSoftwareInstallation::Status>(_status, attributeString("status", _status));
	}

	DriverState::DriverState(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.driverState", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DriverState::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("driverState"); if (!v.is_null()) SetPropertyValueAndNotify<&DriverState::DriverStateValue>(_driverStateValue, v); }
	}

	DriverVersion::DriverVersion(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.driverVersion", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DriverVersion::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&DriverVersion::VersionNumber>(_versionNumber, attributeNumber("versionNumber", _versionNumber));
	}

	EnergyPlanner::EnergyPlanner(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.energyPlanner", version, std::move(componentId), std::move(deviceId), client) {
	}

	void EnergyPlanner::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("data"); if (!v.is_null()) SetPropertyValueAndNotify<&EnergyPlanner::Data>(_data, v); }
		SetPropertyValueAndNotify<&EnergyPlanner::Plan>(_plan, attributeString("plan", _plan));
	}

	Result<void> EnergyPlanner::setPlan(const std::string& plan) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(plan);
		return executeCommand("setPlan", commandArgs);
	}

	Result<void> EnergyPlanner::setData(const nlohmann::json& data) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(data);
		return executeCommand("setData", commandArgs);
	}

	FoodDefrost::FoodDefrost(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.foodDefrost", version, std::move(componentId), std::move(deviceId), client) {
	}

	void FoodDefrost::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("supportedOptions"); if (!v.is_null()) SetPropertyValueAndNotify<&FoodDefrost::SupportedOptions>(_supportedOptions, v); }
		SetPropertyValueAndNotify<&FoodDefrost::FoodType>(_foodType, attributeString("foodType", _foodType));
		{ auto v = attributeValue("weight"); if (!v.is_null()) SetPropertyValueAndNotify<&FoodDefrost::Weight>(_weight, v); }
		SetPropertyValueAndNotify<&FoodDefrost::OperationTime>(_operationTime, static_cast<int>(attributeNumber("operationTime", _operationTime)));
		SetPropertyValueAndNotify<&FoodDefrost::RemainingTime>(_remainingTime, static_cast<int>(attributeNumber("remainingTime", _remainingTime)));
	}

	Result<void> FoodDefrost::setDefrost(const std::string& foodType, const nlohmann::json& weight) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(foodType);
		commandArgs.push_back(weight);
		return executeCommand("setDefrost", commandArgs);
	}

	FreezerConvertMode::FreezerConvertMode(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.freezerConvertMode", version, std::move(componentId), std::move(deviceId), client) {
	}

	void FreezerConvertMode::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedFreezerConvertModes"); if (!v.empty()) SetPropertyValueAndNotify<&FreezerConvertMode::SupportedFreezerConvertModes>(_supportedFreezerConvertModes, v); }
		SetPropertyValueAndNotify<&FreezerConvertMode::FreezerConvertModeValue>(_freezerConvertModeValue, attributeString("freezerConvertMode", _freezerConvertModeValue));
	}

	Result<void> FreezerConvertMode::setFreezerConvertMode(const std::string& freezerConvertMode) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(freezerConvertMode);
		return executeCommand("setFreezerConvertMode", commandArgs);
	}

	FridgeIcemakerInfo::FridgeIcemakerInfo(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.fridgeIcemakerInfo", version, std::move(componentId), std::move(deviceId), client) {
	}

	void FridgeIcemakerInfo::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&FridgeIcemakerInfo::Name>(_name, attributeString("name", _name));
	}

	FridgePantryInfo::FridgePantryInfo(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.fridgePantryInfo", version, std::move(componentId), std::move(deviceId), client) {
	}

	void FridgePantryInfo::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&FridgePantryInfo::Name>(_name, attributeString("name", _name));
	}

	FridgePantryMode::FridgePantryMode(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.fridgePantryMode", version, std::move(componentId), std::move(deviceId), client) {
	}

	void FridgePantryMode::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&FridgePantryMode::Mode>(_mode, attributeString("mode", _mode));
		{ auto v = attributeStringArray("supportedModes"); if (!v.empty()) SetPropertyValueAndNotify<&FridgePantryMode::SupportedModes>(_supportedModes, v); }
	}

	Result<void> FridgePantryMode::setMode(const std::string& mode) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(mode);
		return executeCommand("setMode", commandArgs);
	}

	FridgeVacationMode::FridgeVacationMode(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.fridgeVacationMode", version, std::move(componentId), std::move(deviceId), client) {
	}

	void FridgeVacationMode::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&FridgeVacationMode::VacationMode>(_vacationMode, attributeString("vacationMode", _vacationMode));
	}

	FridgeWelcomeLighting::FridgeWelcomeLighting(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.fridgeWelcomeLighting", version, std::move(componentId), std::move(deviceId), client) {
	}

	void FridgeWelcomeLighting::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&FridgeWelcomeLighting::DetectionProximity>(_detectionProximity, attributeString("detectionProximity", _detectionProximity));
		{ auto v = attributeStringArray("supportedDetectionProximities"); if (!v.empty()) SetPropertyValueAndNotify<&FridgeWelcomeLighting::SupportedDetectionProximities>(_supportedDetectionProximities, v); }
		SetPropertyValueAndNotify<&FridgeWelcomeLighting::Status>(_status, attributeString("status", _status));
	}

	Result<void> FridgeWelcomeLighting::setDetectionProximity(const std::string& detectionProximity) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(detectionProximity);
		return executeCommand("setDetectionProximity", commandArgs);
	}

	Result<void> FridgeWelcomeLighting::off() {
		return executeCommand("off");
	}

	Result<void> FridgeWelcomeLighting::on() {
		return executeCommand("on");
	}

	FridgeZoneInfo::FridgeZoneInfo(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.fridgeZoneInfo", version, std::move(componentId), std::move(deviceId), client) {
	}

	void FridgeZoneInfo::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&FridgeZoneInfo::Name>(_name, attributeString("name", _name));
	}

	KidsLock::KidsLock(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.kidsLock", version, std::move(componentId), std::move(deviceId), client) {
	}

	void KidsLock::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&KidsLock::LockState>(_lockState, attributeString("lockState", _lockState));
	}

	MeatAging::MeatAging(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.meatAging", version, std::move(componentId), std::move(deviceId), client) {
	}

	void MeatAging::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("zoneInfo"); if (!v.is_null()) SetPropertyValueAndNotify<&MeatAging::ZoneInfo>(_zoneInfo, v); }
		{ auto v = attributeStringArray("supportedMeatTypes"); if (!v.empty()) SetPropertyValueAndNotify<&MeatAging::SupportedMeatTypes>(_supportedMeatTypes, v); }
		{ auto v = attributeStringArray("supportedAgingMethods"); if (!v.empty()) SetPropertyValueAndNotify<&MeatAging::SupportedAgingMethods>(_supportedAgingMethods, v); }
		{ auto v = attributeValue("status"); if (!v.is_null()) SetPropertyValueAndNotify<&MeatAging::Status>(_status, v); }
	}

	Result<void> MeatAging::addAging(const nlohmann::json& agingOption) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(agingOption);
		return executeCommand("addAging", commandArgs);
	}

	Result<void> MeatAging::cancelAging(int id) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(id);
		return executeCommand("cancelAging", commandArgs);
	}

	Result<void> MeatAging::updateAging(int id, const nlohmann::json& agingOption) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(id);
		commandArgs.push_back(agingOption);
		return executeCommand("updateAging", commandArgs);
	}

	OperationOrigin::OperationOrigin(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.operationOrigin", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> OperationOrigin::setOperationOrigin(const std::string& origin, const nlohmann::json& data) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(origin);
		commandArgs.push_back(data);
		return executeCommand("setOperationOrigin", commandArgs);
	}

	PowerCool::PowerCool(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.powerCool", version, std::move(componentId), std::move(deviceId), client) {
	}

	void PowerCool::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&PowerCool::Activated>(_activated, attributeBool("activated", _activated));
	}

	Result<void> PowerCool::activate() {
		return executeCommand("activate");
	}

	Result<void> PowerCool::deactivate() {
		return executeCommand("deactivate");
	}

	PowerFreeze::PowerFreeze(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.powerFreeze", version, std::move(componentId), std::move(deviceId), client) {
	}

	void PowerFreeze::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&PowerFreeze::Activated>(_activated, attributeBool("activated", _activated));
	}

	Result<void> PowerFreeze::activate() {
		return executeCommand("activate");
	}

	Result<void> PowerFreeze::deactivate() {
		return executeCommand("deactivate");
	}

	QuickControl::QuickControl(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.quickControl", version, std::move(componentId), std::move(deviceId), client) {
	}

	void QuickControl::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&QuickControl::VersionAttr>(_versionAttr, attributeString("version", _versionAttr));
	}

	SabbathMode::SabbathMode(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.sabbathMode", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SabbathMode::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedActions"); if (!v.empty()) SetPropertyValueAndNotify<&SabbathMode::SupportedActions>(_supportedActions, v); }
		SetPropertyValueAndNotify<&SabbathMode::Status>(_status, attributeString("status", _status));
	}

	Result<void> SabbathMode::off() {
		return executeCommand("off");
	}

	Result<void> SabbathMode::on() {
		return executeCommand("on");
	}

	SelfCheck::SelfCheck(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.selfCheck", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SelfCheck::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&SelfCheck::ResultAttr>(_resultAttr, attributeString("result", _resultAttr));
		{ auto v = attributeStringArray("supportedActions"); if (!v.empty()) SetPropertyValueAndNotify<&SelfCheck::SupportedActions>(_supportedActions, v); }
		SetPropertyValueAndNotify<&SelfCheck::Progress>(_progress, static_cast<int>(attributeNumber("progress", _progress)));
		{ auto v = attributeValue("errors"); if (!v.is_null()) SetPropertyValueAndNotify<&SelfCheck::Errors>(_errors, v); }
		SetPropertyValueAndNotify<&SelfCheck::Status>(_status, attributeString("status", _status));
	}

	Result<void> SelfCheck::cancelSelfCheck() {
		return executeCommand("cancelSelfCheck");
	}

	Result<void> SelfCheck::startSelfCheck() {
		return executeCommand("startSelfCheck");
	}

	SoftenerAutoReplenishment::SoftenerAutoReplenishment(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.softenerAutoReplenishment", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SoftenerAutoReplenishment::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&SoftenerAutoReplenishment::RegularSoftenerType>(_regularSoftenerType, attributeString("regularSoftenerType", _regularSoftenerType));
		SetPropertyValueAndNotify<&SoftenerAutoReplenishment::RegularSoftenerAlarmEnabled>(_regularSoftenerAlarmEnabled, attributeBool("regularSoftenerAlarmEnabled", _regularSoftenerAlarmEnabled));
		SetPropertyValueAndNotify<&SoftenerAutoReplenishment::RegularSoftenerInitialAmount>(_regularSoftenerInitialAmount, attributeNumber("regularSoftenerInitialAmount", _regularSoftenerInitialAmount));
		SetPropertyValueAndNotify<&SoftenerAutoReplenishment::RegularSoftenerRemainingAmount>(_regularSoftenerRemainingAmount, attributeNumber("regularSoftenerRemainingAmount", _regularSoftenerRemainingAmount));
		SetPropertyValueAndNotify<&SoftenerAutoReplenishment::RegularSoftenerDosage>(_regularSoftenerDosage, attributeNumber("regularSoftenerDosage", _regularSoftenerDosage));
		SetPropertyValueAndNotify<&SoftenerAutoReplenishment::RegularSoftenerOrderThreshold>(_regularSoftenerOrderThreshold, attributeNumber("regularSoftenerOrderThreshold", _regularSoftenerOrderThreshold));
	}

	Result<void> SoftenerAutoReplenishment::enableAlarm(const std::string& name) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		return executeCommand("enableAlarm", commandArgs);
	}

	Result<void> SoftenerAutoReplenishment::setOrderThreshold(const std::string& name, double threshold) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		commandArgs.push_back(threshold);
		return executeCommand("setOrderThreshold", commandArgs);
	}

	Result<void> SoftenerAutoReplenishment::disableAlarm(const std::string& name) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		return executeCommand("disableAlarm", commandArgs);
	}

	Result<void> SoftenerAutoReplenishment::setAutoReplenishment(const nlohmann::json& autoReplenishment) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(autoReplenishment);
		return executeCommand("setAutoReplenishment", commandArgs);
	}

	Result<void> SoftenerAutoReplenishment::setInitialAmount(const std::string& name, double amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		commandArgs.push_back(amount);
		return executeCommand("setInitialAmount", commandArgs);
	}

	Result<void> SoftenerAutoReplenishment::setRemainingAmount(const std::string& name, double amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		commandArgs.push_back(amount);
		return executeCommand("setRemainingAmount", commandArgs);
	}

	Result<void> SoftenerAutoReplenishment::setDosage(const std::string& name, double amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		commandArgs.push_back(amount);
		return executeCommand("setDosage", commandArgs);
	}

	Result<void> SoftenerAutoReplenishment::setType(const std::string& name, const std::string& type) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(name);
		commandArgs.push_back(type);
		return executeCommand("setType", commandArgs);
	}

	SoftenerOrder::SoftenerOrder(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.softenerOrder", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SoftenerOrder::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&SoftenerOrder::AlarmEnabled>(_alarmEnabled, attributeBool("alarmEnabled", _alarmEnabled));
		SetPropertyValueAndNotify<&SoftenerOrder::OrderThreshold>(_orderThreshold, attributeNumber("orderThreshold", _orderThreshold));
	}

	Result<void> SoftenerOrder::enableAlarm() {
		return executeCommand("enableAlarm");
	}

	Result<void> SoftenerOrder::setOrderThreshold(double threshold) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(threshold);
		return executeCommand("setOrderThreshold", commandArgs);
	}

	Result<void> SoftenerOrder::disableAlarm() {
		return executeCommand("disableAlarm");
	}

	SoftenerState::SoftenerState(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.softenerState", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SoftenerState::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&SoftenerState::RemainingAmount>(_remainingAmount, attributeNumber("remainingAmount", _remainingAmount));
		SetPropertyValueAndNotify<&SoftenerState::Dosage>(_dosage, attributeNumber("dosage", _dosage));
		SetPropertyValueAndNotify<&SoftenerState::SoftenerType>(_softenerType, attributeString("softenerType", _softenerType));
		SetPropertyValueAndNotify<&SoftenerState::InitialAmount>(_initialAmount, attributeNumber("initialAmount", _initialAmount));
	}

	Result<void> SoftenerState::setInitialAmount(double amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(amount);
		return executeCommand("setInitialAmount", commandArgs);
	}

	Result<void> SoftenerState::setRemainingAmount(double amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(amount);
		return executeCommand("setRemainingAmount", commandArgs);
	}

	Result<void> SoftenerState::setDosage(double amount) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(amount);
		return executeCommand("setDosage", commandArgs);
	}

	Result<void> SoftenerState::setSoftenerType(const std::string& softenerType) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(softenerType);
		return executeCommand("setSoftenerType", commandArgs);
	}

	SoftwareUpdate::SoftwareUpdate(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.softwareUpdate", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SoftwareUpdate::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("targetModule"); if (!v.is_null()) SetPropertyValueAndNotify<&SoftwareUpdate::TargetModule>(_targetModule, v); }
		SetPropertyValueAndNotify<&SoftwareUpdate::OtnDUID>(_otnDUID, attributeString("otnDUID", _otnDUID));
		{ auto v = attributeValue("schedule"); if (!v.is_null()) SetPropertyValueAndNotify<&SoftwareUpdate::Schedule>(_schedule, v); }
		{ auto v = attributeValue("availableModuleDetails"); if (!v.is_null()) SetPropertyValueAndNotify<&SoftwareUpdate::AvailableModuleDetails>(_availableModuleDetails, v); }
		SetPropertyValueAndNotify<&SoftwareUpdate::LastUpdatedDate>(_lastUpdatedDate, attributeString("lastUpdatedDate", _lastUpdatedDate));
		{ auto v = attributeStringArray("availableModules"); if (!v.empty()) SetPropertyValueAndNotify<&SoftwareUpdate::AvailableModules>(_availableModules, v); }
		SetPropertyValueAndNotify<&SoftwareUpdate::NewVersionAvailable>(_newVersionAvailable, attributeBool("newVersionAvailable", _newVersionAvailable));
		SetPropertyValueAndNotify<&SoftwareUpdate::OperatingState>(_operatingState, attributeString("operatingState", _operatingState));
		SetPropertyValueAndNotify<&SoftwareUpdate::Progress>(_progress, static_cast<int>(attributeNumber("progress", _progress)));
		SetPropertyValueAndNotify<&SoftwareUpdate::ProtocolVersion>(_protocolVersion, attributeString("protocolVersion", _protocolVersion));
		{ auto v = attributeValue("moduleUpdateCounts"); if (!v.is_null()) SetPropertyValueAndNotify<&SoftwareUpdate::ModuleUpdateCounts>(_moduleUpdateCounts, v); }
	}

	Result<void> SoftwareUpdate::setSchedule(const nlohmann::json& schedule) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(schedule);
		return executeCommand("setSchedule", commandArgs);
	}

	Result<void> SoftwareUpdate::agreeUpdate(const std::string& module) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(module);
		return executeCommand("agreeUpdate", commandArgs);
	}

	Result<void> SoftwareUpdate::disagreeUpdate(const std::string& module) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(module);
		return executeCommand("disagreeUpdate", commandArgs);
	}

	Result<void> SoftwareUpdate::cancelSchedule() {
		return executeCommand("cancelSchedule");
	}

	SoftwareVersion::SoftwareVersion(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.softwareVersion", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SoftwareVersion::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("versions"); if (!v.is_null()) SetPropertyValueAndNotify<&SoftwareVersion::Versions>(_versions, v); }
		{ auto v = attributeValue("platformVersion"); if (!v.is_null()) SetPropertyValueAndNotify<&SoftwareVersion::PlatformVersion>(_platformVersion, v); }
	}

	TemperatureSetting::TemperatureSetting(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.temperatureSetting", version, std::move(componentId), std::move(deviceId), client) {
	}

	void TemperatureSetting::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("supportedDesiredTemperatures"); if (!v.is_null()) SetPropertyValueAndNotify<&TemperatureSetting::SupportedDesiredTemperatures>(_supportedDesiredTemperatures, v); }
		SetPropertyValueAndNotify<&TemperatureSetting::DesiredTemperature>(_desiredTemperature, attributeNumber("desiredTemperature", _desiredTemperature));
	}

	Result<void> TemperatureSetting::setDesiredTemperature(double desiredTemperature) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(desiredTemperature);
		return executeCommand("setDesiredTemperature", commandArgs);
	}

	UnavailableCapabilities::UnavailableCapabilities(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.unavailableCapabilities", version, std::move(componentId), std::move(deviceId), client) {
	}

	void UnavailableCapabilities::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("unavailableCommands"); if (!v.empty()) SetPropertyValueAndNotify<&UnavailableCapabilities::UnavailableCommands>(_unavailableCommands, v); }
	}

	ViewInside::ViewInside(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.viewInside", version, std::move(componentId), std::move(deviceId), client) {
	}

	void ViewInside::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedFocusAreas"); if (!v.empty()) SetPropertyValueAndNotify<&ViewInside::SupportedFocusAreas>(_supportedFocusAreas, v); }
		{ auto v = attributeValue("contents"); if (!v.is_null()) SetPropertyValueAndNotify<&ViewInside::Contents>(_contents, v); }
		SetPropertyValueAndNotify<&ViewInside::LastUpdatedTime>(_lastUpdatedTime, attributeString("lastUpdatedTime", _lastUpdatedTime));
	}

	Result<void> ViewInside::refreshSpecificArea(const std::string& focusArea) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(focusArea);
		return executeCommand("refreshSpecificArea", commandArgs);
	}

	Result<void> ViewInside::refreshAll() {
		return executeCommand("refreshAll");
	}

	Result<void> ViewInside::refresh(const std::string& fileId) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(fileId);
		return executeCommand("refresh", commandArgs);
	}

	WasherBubbleSoak::WasherBubbleSoak(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.washerBubbleSoak", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherBubbleSoak::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WasherBubbleSoak::Status>(_status, attributeString("status", _status));
	}

	Result<void> WasherBubbleSoak::off() {
		return executeCommand("off");
	}

	Result<void> WasherBubbleSoak::on() {
		return executeCommand("on");
	}

	WasherCycle::WasherCycle(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.washerCycle", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherCycle::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WasherCycle::CycleType>(_cycleType, attributeString("cycleType", _cycleType));
		{ auto v = attributeValue("supportedCycles"); if (!v.is_null()) SetPropertyValueAndNotify<&WasherCycle::SupportedCycles>(_supportedCycles, v); }
		{ auto v = attributeStringArray("availableCycleTypes"); if (!v.empty()) SetPropertyValueAndNotify<&WasherCycle::AvailableCycleTypes>(_availableCycleTypes, v); }
		SetPropertyValueAndNotify<&WasherCycle::WasherCycleValue>(_washerCycleValue, attributeString("washerCycle", _washerCycleValue));
		{ auto v = attributeValue("referenceTable"); if (!v.is_null()) SetPropertyValueAndNotify<&WasherCycle::ReferenceTable>(_referenceTable, v); }
		SetPropertyValueAndNotify<&WasherCycle::SpecializedFunctionClassification>(_specializedFunctionClassification, static_cast<int>(attributeNumber("specializedFunctionClassification", _specializedFunctionClassification)));
	}

	Result<void> WasherCycle::setWasherCycle(const std::string& washerCycle) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(washerCycle);
		return executeCommand("setWasherCycle", commandArgs);
	}

	Result<void> WasherCycle::setCycleType(const std::string& cycleType, const std::string& washerCycle) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(cycleType);
		commandArgs.push_back(washerCycle);
		return executeCommand("setCycleType", commandArgs);
	}

	WasherCyclePreset::WasherCyclePreset(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.washerCyclePreset", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherCyclePreset::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WasherCyclePreset::MaxNumberOfPresets>(_maxNumberOfPresets, attributeNumber("maxNumberOfPresets", _maxNumberOfPresets));
		{ auto v = attributeValue("presets"); if (!v.is_null()) SetPropertyValueAndNotify<&WasherCyclePreset::Presets>(_presets, v); }
	}

	Result<void> WasherCyclePreset::setWasherCyclePreset(const nlohmann::json& presets) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(presets);
		return executeCommand("setWasherCyclePreset", commandArgs);
	}

	Result<void> WasherCyclePreset::delete_(const nlohmann::json& presetIds) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(presetIds);
		return executeCommand("delete", commandArgs);
	}

	WasherDelayEnd::WasherDelayEnd(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.washerDelayEnd", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherDelayEnd::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WasherDelayEnd::RemainingTime>(_remainingTime, static_cast<int>(attributeNumber("remainingTime", _remainingTime)));
		SetPropertyValueAndNotify<&WasherDelayEnd::MinimumReservableTime>(_minimumReservableTime, static_cast<int>(attributeNumber("minimumReservableTime", _minimumReservableTime)));
	}

	Result<void> WasherDelayEnd::refreshMinimumReservableTime() {
		return executeCommand("refreshMinimumReservableTime");
	}

	Result<void> WasherDelayEnd::setDelayTime(int delayTime) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(delayTime);
		return executeCommand("setDelayTime", commandArgs);
	}

	WasherFreezePrevent::WasherFreezePrevent(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.washerFreezePrevent", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherFreezePrevent::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WasherFreezePrevent::OperatingState>(_operatingState, attributeString("operatingState", _operatingState));
	}

	WasherOperatingState::WasherOperatingState(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.washerOperatingState", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherOperatingState::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WasherOperatingState::WashingProgress>(_washingProgress, static_cast<int>(attributeNumber("washingProgress", _washingProgress)));
		SetPropertyValueAndNotify<&WasherOperatingState::DryingProgress>(_dryingProgress, static_cast<int>(attributeNumber("dryingProgress", _dryingProgress)));
		SetPropertyValueAndNotify<&WasherOperatingState::WasherJobState>(_washerJobState, attributeString("washerJobState", _washerJobState));
		SetPropertyValueAndNotify<&WasherOperatingState::OperatingState>(_operatingState, attributeString("operatingState", _operatingState));
		{ auto v = attributeStringArray("supportedOperatingStates"); if (!v.empty()) SetPropertyValueAndNotify<&WasherOperatingState::SupportedOperatingStates>(_supportedOperatingStates, v); }
		{ auto v = attributeValue("scheduledJobs"); if (!v.is_null()) SetPropertyValueAndNotify<&WasherOperatingState::ScheduledJobs>(_scheduledJobs, v); }
		{ auto v = attributeValue("scheduledPhases"); if (!v.is_null()) SetPropertyValueAndNotify<&WasherOperatingState::ScheduledPhases>(_scheduledPhases, v); }
		SetPropertyValueAndNotify<&WasherOperatingState::Progress>(_progress, static_cast<int>(attributeNumber("progress", _progress)));
		SetPropertyValueAndNotify<&WasherOperatingState::RemainingTimeStr>(_remainingTimeStr, attributeString("remainingTimeStr", _remainingTimeStr));
		SetPropertyValueAndNotify<&WasherOperatingState::WasherJobPhase>(_washerJobPhase, attributeString("washerJobPhase", _washerJobPhase));
		SetPropertyValueAndNotify<&WasherOperatingState::OperationTime>(_operationTime, static_cast<int>(attributeNumber("operationTime", _operationTime)));
		SetPropertyValueAndNotify<&WasherOperatingState::RemainingTime>(_remainingTime, static_cast<int>(attributeNumber("remainingTime", _remainingTime)));
	}

	Result<void> WasherOperatingState::resume() {
		return executeCommand("resume");
	}

	Result<void> WasherOperatingState::cancel() {
		return executeCommand("cancel");
	}

	Result<void> WasherOperatingState::start() {
		return executeCommand("start");
	}

	Result<void> WasherOperatingState::estimateOperationTime() {
		return executeCommand("estimateOperationTime");
	}

	Result<void> WasherOperatingState::setDelayEnd(int delayTime) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(delayTime);
		return executeCommand("setDelayEnd", commandArgs);
	}

	Result<void> WasherOperatingState::pause() {
		return executeCommand("pause");
	}

	WasherWashingTime::WasherWashingTime(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.washerWashingTime", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherWashingTime::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedWashingTimes"); if (!v.empty()) SetPropertyValueAndNotify<&WasherWashingTime::SupportedWashingTimes>(_supportedWashingTimes, v); }
		SetPropertyValueAndNotify<&WasherWashingTime::WashingTime>(_washingTime, attributeString("washingTime", _washingTime));
	}

	Result<void> WasherWashingTime::setWashingTime(const std::string& washingTime) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(washingTime);
		return executeCommand("setWashingTime", commandArgs);
	}

	WasherWaterLevel::WasherWaterLevel(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.washerWaterLevel", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherWaterLevel::parseStatus(const nlohmann::json&) {
		{ auto v = attributeStringArray("supportedWaterLevel"); if (!v.empty()) SetPropertyValueAndNotify<&WasherWaterLevel::SupportedWaterLevel>(_supportedWaterLevel, v); }
		SetPropertyValueAndNotify<&WasherWaterLevel::WaterLevel>(_waterLevel, attributeString("waterLevel", _waterLevel));
	}

	Result<void> WasherWaterLevel::setWaterLevel(const std::string& waterLevel) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(waterLevel);
		return executeCommand("setWaterLevel", commandArgs);
	}

	WasherWaterValve::WasherWaterValve(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.washerWaterValve", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherWaterValve::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WasherWaterValve::WaterValve>(_waterValve, attributeString("waterValve", _waterValve));
		{ auto v = attributeStringArray("supportedWaterValve"); if (!v.empty()) SetPropertyValueAndNotify<&WasherWaterValve::SupportedWaterValve>(_supportedWaterValve, v); }
	}

	Result<void> WasherWaterValve::setWaterValve(const std::string& waterValve) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(waterValve);
		return executeCommand("setWaterValve", commandArgs);
	}

	WaterConsumptionReport::WaterConsumptionReport(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.waterConsumptionReport", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WaterConsumptionReport::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("waterConsumption"); if (!v.is_null()) SetPropertyValueAndNotify<&WaterConsumptionReport::WaterConsumption>(_waterConsumption, v); }
	}

	WeightMeasurement::WeightMeasurement(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.weightMeasurement", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WeightMeasurement::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WeightMeasurement::Weight>(_weight, attributeNumber("weight", _weight));
	}

	WeightMeasurementCalibration::WeightMeasurementCalibration(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.weightMeasurementCalibration", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> WeightMeasurementCalibration::zeroCalibrate() {
		return executeCommand("zeroCalibrate");
	}

	WelcomeMessage::WelcomeMessage(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungce.welcomeMessage", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WelcomeMessage::parseStatus(const nlohmann::json&) {
		{ auto v = attributeValue("welcomeMessage"); if (!v.is_null()) SetPropertyValueAndNotify<&WelcomeMessage::WelcomeMessageValue>(_welcomeMessageValue, v); }
	}

	Result<void> WelcomeMessage::deleteWelcomeMessage() {
		return executeCommand("deleteWelcomeMessage");
	}

	Result<void> WelcomeMessage::setWelcomeMessage(const nlohmann::json& welcomeMessage) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(welcomeMessage);
		return executeCommand("setWelcomeMessage", commandArgs);
	}

} // namespace samsungce
} // namespace smartthings4cpp
