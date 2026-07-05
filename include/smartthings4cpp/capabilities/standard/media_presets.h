#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c mediaPresets capability (generated). */
	class MediaPresets : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "mediaPresets";

		MediaPresets(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Presets{ [this]() { return _presets; } };

		Result<void> playPreset(const std::string& presetId);
		std::vector<std::string> commandNames() const override { return { "playPreset" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _presets;
	};

} // namespace standard
} // namespace smartthings4cpp
