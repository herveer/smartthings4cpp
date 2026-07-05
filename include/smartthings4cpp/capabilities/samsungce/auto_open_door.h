#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.autoOpenDoor capability (Samsung-proprietary; generated). */
	class AutoOpenDoor : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.autoOpenDoor";

		AutoOpenDoor(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> AutoOpenDoorValue = MakeReadonlyProperty<&AutoOpenDoor::AutoOpenDoorValue>([this]() { return _autoOpenDoorValue; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedPressureLevels = MakeReadonlyProperty<&AutoOpenDoor::SupportedPressureLevels>([this]() { return _supportedPressureLevels; });
		ReactiveLitepp::ReadonlyProperty<std::string> PressureLevel = MakeReadonlyProperty<&AutoOpenDoor::PressureLevel>([this]() { return _pressureLevel; });

		Result<void> off();
		Result<void> on();
		Result<void> setPressureLevel(const std::string& pressureLevel);
		std::vector<std::string> commandNames() const override { return { "off", "on", "setPressureLevel" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _autoOpenDoorValue;
		std::vector<std::string> _supportedPressureLevels;
		std::string _pressureLevel;
	};

} // namespace samsungce
} // namespace smartthings4cpp
