#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.dishwasherWashingOptions capability (Samsung-proprietary; generated). */
	class DishwasherWashingOptions : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.dishwasherWashingOptions";

		DishwasherWashingOptions(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> DryPlus{ [this]() { return _dryPlus; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> StormWash{ [this]() { return _stormWash; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> MultiTab{ [this]() { return _multiTab; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> HotAirDry{ [this]() { return _hotAirDry; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SelectedZone{ [this]() { return _selectedZone; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SpeedBooster{ [this]() { return _speedBooster; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> HighTempWash{ [this]() { return _highTempWash; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SanitizingWash{ [this]() { return _sanitizingWash; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> HeatedDry{ [this]() { return _heatedDry; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ZoneBooster{ [this]() { return _zoneBooster; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> AddRinse{ [this]() { return _addRinse; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedList{ [this]() { return _supportedList; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> RinsePlus{ [this]() { return _rinsePlus; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Sanitize{ [this]() { return _sanitize; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SteamSoak{ [this]() { return _steamSoak; } };

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
