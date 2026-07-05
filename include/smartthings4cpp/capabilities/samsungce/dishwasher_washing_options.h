#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.dishwasherWashingOptions capability (Samsung-proprietary; generated). */
	class DishwasherWashingOptions : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.dishwasherWashingOptions";

		DishwasherWashingOptions(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> DryPlus = MakeReadonlyProperty<&DishwasherWashingOptions::DryPlus>([this]() { return _dryPlus; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> StormWash = MakeReadonlyProperty<&DishwasherWashingOptions::StormWash>([this]() { return _stormWash; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> MultiTab = MakeReadonlyProperty<&DishwasherWashingOptions::MultiTab>([this]() { return _multiTab; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> HotAirDry = MakeReadonlyProperty<&DishwasherWashingOptions::HotAirDry>([this]() { return _hotAirDry; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SelectedZone = MakeReadonlyProperty<&DishwasherWashingOptions::SelectedZone>([this]() { return _selectedZone; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SpeedBooster = MakeReadonlyProperty<&DishwasherWashingOptions::SpeedBooster>([this]() { return _speedBooster; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> HighTempWash = MakeReadonlyProperty<&DishwasherWashingOptions::HighTempWash>([this]() { return _highTempWash; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SanitizingWash = MakeReadonlyProperty<&DishwasherWashingOptions::SanitizingWash>([this]() { return _sanitizingWash; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> HeatedDry = MakeReadonlyProperty<&DishwasherWashingOptions::HeatedDry>([this]() { return _heatedDry; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ZoneBooster = MakeReadonlyProperty<&DishwasherWashingOptions::ZoneBooster>([this]() { return _zoneBooster; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> AddRinse = MakeReadonlyProperty<&DishwasherWashingOptions::AddRinse>([this]() { return _addRinse; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedList = MakeReadonlyProperty<&DishwasherWashingOptions::SupportedList>([this]() { return _supportedList; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> RinsePlus = MakeReadonlyProperty<&DishwasherWashingOptions::RinsePlus>([this]() { return _rinsePlus; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Sanitize = MakeReadonlyProperty<&DishwasherWashingOptions::Sanitize>([this]() { return _sanitize; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SteamSoak = MakeReadonlyProperty<&DishwasherWashingOptions::SteamSoak>([this]() { return _steamSoak; });

		Result<void> setSelectedZone(const std::string& selectedZone);
		Result<void> setSanitize(bool sanitize);
		Result<void> setZoneBooster(const std::string& zoneBooster);
		Result<void> setDryPlus(bool dryPlus);
		Result<void> setAddRinse(bool addRinse);
		Result<void> setHotAirDry(bool hotAirDry);
		Result<void> setStormWash(bool stormWash);
		Result<void> setHeatedDry(const std::string& heatedDry);
		Result<void> setHighTempWash(bool highTempWash);
		Result<void> setSanitizingWash(bool sanitizingWash);
		Result<void> setMultiTab(bool multiTab);
		Result<void> setSpeedBooster(bool speedBooster);
		Result<void> setRinsePlus(bool rinsePlus);
		Result<void> setOptions(const nlohmann::json& options);
		Result<void> setSteamSoak(bool steamSoak);
		std::vector<std::string> commandNames() const override { return { "setSelectedZone", "setSanitize", "setZoneBooster", "setDryPlus", "setAddRinse", "setHotAirDry", "setStormWash", "setHeatedDry", "setHighTempWash", "setSanitizingWash", "setMultiTab", "setSpeedBooster", "setRinsePlus", "setOptions", "setSteamSoak" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _dryPlus;
		nlohmann::json _stormWash;
		nlohmann::json _multiTab;
		nlohmann::json _hotAirDry;
		nlohmann::json _selectedZone;
		nlohmann::json _speedBooster;
		nlohmann::json _highTempWash;
		nlohmann::json _sanitizingWash;
		nlohmann::json _heatedDry;
		nlohmann::json _zoneBooster;
		nlohmann::json _addRinse;
		std::vector<std::string> _supportedList;
		nlohmann::json _rinsePlus;
		nlohmann::json _sanitize;
		nlohmann::json _steamSoak;
	};

} // namespace samsungce
} // namespace smartthings4cpp
