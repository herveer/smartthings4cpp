#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.softwareUpdate capability (Samsung-proprietary; generated). */
	class SoftwareUpdate : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.softwareUpdate";

		SoftwareUpdate(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> TargetModule{ [this]() { return _targetModule; } };
		ReactiveLitepp::ReadonlyProperty<std::string> OtnDUID{ [this]() { return _otnDUID; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Schedule{ [this]() { return _schedule; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> AvailableModuleDetails{ [this]() { return _availableModuleDetails; } };
		ReactiveLitepp::ReadonlyProperty<std::string> LastUpdatedDate{ [this]() { return _lastUpdatedDate; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> AvailableModules{ [this]() { return _availableModules; } };
		ReactiveLitepp::ReadonlyProperty<bool> NewVersionAvailable{ [this]() { return _newVersionAvailable; } };
		ReactiveLitepp::ReadonlyProperty<std::string> OperatingState{ [this]() { return _operatingState; } };
		ReactiveLitepp::ReadonlyProperty<int> Progress{ [this]() { return _progress; } };
		ReactiveLitepp::ReadonlyProperty<std::string> ProtocolVersion{ [this]() { return _protocolVersion; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ModuleUpdateCounts{ [this]() { return _moduleUpdateCounts; } };

		Result<void> setSchedule(const nlohmann::json& schedule);
		Result<void> agreeUpdate(const std::string& module);
		Result<void> disagreeUpdate(const std::string& module);
		Result<void> cancelSchedule();
		std::vector<std::string> commandNames() const override { return { "setSchedule", "agreeUpdate", "disagreeUpdate", "cancelSchedule" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _targetModule;
		std::string _otnDUID;
		nlohmann::json _schedule;
		nlohmann::json _availableModuleDetails;
		std::string _lastUpdatedDate;
		std::vector<std::string> _availableModules;
		bool _newVersionAvailable = false;
		std::string _operatingState;
		int _progress = 0;
		std::string _protocolVersion;
		nlohmann::json _moduleUpdateCounts;
	};

} // namespace samsungce
} // namespace smartthings4cpp
