#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.washerWaterTemperature capability (Samsung-proprietary; generated). */
	class WasherWaterTemperature : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.washerWaterTemperature";

		WasherWaterTemperature(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedWasherWaterTemperature = MakeReadonlyProperty<&WasherWaterTemperature::SupportedWasherWaterTemperature>([this]() { return _supportedWasherWaterTemperature; });
		ReactiveLitepp::ReadonlyProperty<std::string> WasherWaterTemperatureValue = MakeReadonlyProperty<&WasherWaterTemperature::WasherWaterTemperatureValue>([this]() { return _washerWaterTemperatureValue; });

		Result<void> setWasherWaterTemperature(const std::string& temperature);
		std::vector<std::string> commandNames() const override { return { "setWasherWaterTemperature" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedWasherWaterTemperature;
		std::string _washerWaterTemperatureValue;
	};

} // namespace custom
} // namespace smartthings4cpp
