#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.energyType capability (Samsung-proprietary; generated). */
	class EnergyType : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.energyType";

		EnergyType(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> EnergyTypeValue = MakeReadonlyProperty<&EnergyType::EnergyTypeValue>([this]() { return _energyTypeValue; });
		ReactiveLitepp::ReadonlyProperty<bool> EnergySavingSupport = MakeReadonlyProperty<&EnergyType::EnergySavingSupport>([this]() { return _energySavingSupport; });
		ReactiveLitepp::ReadonlyProperty<int> DrMaxDuration = MakeReadonlyProperty<&EnergyType::DrMaxDuration>([this]() { return _drMaxDuration; });
		ReactiveLitepp::ReadonlyProperty<int> EnergySavingLevel = MakeReadonlyProperty<&EnergyType::EnergySavingLevel>([this]() { return _energySavingLevel; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> EnergySavingInfo = MakeReadonlyProperty<&EnergyType::EnergySavingInfo>([this]() { return _energySavingInfo; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedEnergySavingLevels = MakeReadonlyProperty<&EnergyType::SupportedEnergySavingLevels>([this]() { return _supportedEnergySavingLevels; });
		ReactiveLitepp::ReadonlyProperty<bool> EnergySavingOperation = MakeReadonlyProperty<&EnergyType::EnergySavingOperation>([this]() { return _energySavingOperation; });
		ReactiveLitepp::ReadonlyProperty<std::string> NotificationTemplateID = MakeReadonlyProperty<&EnergyType::NotificationTemplateID>([this]() { return _notificationTemplateID; });
		ReactiveLitepp::ReadonlyProperty<bool> EnergySavingOperationSupport = MakeReadonlyProperty<&EnergyType::EnergySavingOperationSupport>([this]() { return _energySavingOperationSupport; });

		Result<void> setEnergySavingLevel(int energySavingLevel);
		std::vector<std::string> commandNames() const override { return { "setEnergySavingLevel" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _energyTypeValue;
		bool _energySavingSupport = false;
		int _drMaxDuration = 0;
		int _energySavingLevel = 0;
		nlohmann::json _energySavingInfo;
		nlohmann::json _supportedEnergySavingLevels;
		bool _energySavingOperation = false;
		std::string _notificationTemplateID;
		bool _energySavingOperationSupport = false;
	};

} // namespace custom
} // namespace smartthings4cpp
