#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.washerWaterLevel capability (Samsung-proprietary; generated). */
	class WasherWaterLevel : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.washerWaterLevel";

		WasherWaterLevel(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedWaterLevel = MakeReadonlyProperty<&WasherWaterLevel::SupportedWaterLevel>([this]() { return _supportedWaterLevel; });
		ReactiveLitepp::ReadonlyProperty<std::string> WaterLevel = MakeReadonlyProperty<&WasherWaterLevel::WaterLevel>([this]() { return _waterLevel; });

		Result<void> setWaterLevel(const std::string& waterLevel);
		std::vector<std::string> commandNames() const override { return { "setWaterLevel" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedWaterLevel;
		std::string _waterLevel;
	};

} // namespace samsungce
} // namespace smartthings4cpp
