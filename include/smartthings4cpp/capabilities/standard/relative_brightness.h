#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c relativeBrightness capability (generated). */
	class RelativeBrightness : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "relativeBrightness";

		RelativeBrightness(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> BrightnessIntensity{ [this]() { return _brightnessIntensity; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _brightnessIntensity = 0;
	};

} // namespace standard
} // namespace smartthings4cpp
