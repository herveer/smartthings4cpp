#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c colorControl capability (generated). */
	class ColorControl : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "colorControl";

		ColorControl(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<double> Saturation{ [this]() { return _saturation; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Color{ [this]() { return _color; } };
		ReactiveLitepp::ReadonlyProperty<double> Hue{ [this]() { return _hue; } };

		Result<void> setColor(const nlohmann::json& color);
		Result<void> setHue(double hue);
		Result<void> setSaturation(double saturation);
		std::vector<std::string> commandNames() const override { return { "setColor", "setHue", "setSaturation" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		double _saturation = 0.0;
		std::string _color;
		double _hue = 0.0;
	};

} // namespace standard
} // namespace smartthings4cpp
