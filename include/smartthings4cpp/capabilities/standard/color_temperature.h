#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c colorTemperature capability (generated). */
	class ColorTemperature : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "colorTemperature";

		ColorTemperature(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ColorTemperatureRange{ [this]() { return _colorTemperatureRange; } };
		ReactiveLitepp::ReadonlyProperty<int> ColorTemperatureValue{ [this]() { return _colorTemperatureValue; } };

		Result<void> setColorTemperature(int temperature);
		std::vector<std::string> commandNames() const override { return { "setColorTemperature" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _colorTemperatureRange;
		int _colorTemperatureValue = 0;
	};

} // namespace standard
} // namespace smartthings4cpp
