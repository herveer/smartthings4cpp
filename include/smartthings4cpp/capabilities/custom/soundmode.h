#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.soundmode capability (Samsung-proprietary; generated). */
	class Soundmode : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.soundmode";

		Soundmode(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedSoundModesMap{ [this]() { return _supportedSoundModesMap; } };
		ReactiveLitepp::ReadonlyProperty<std::string> SoundModeValue{ [this]() { return _soundModeValue; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedSoundModes{ [this]() { return _supportedSoundModes; } };

		Result<void> setSoundMode(const std::string& mode);
		std::vector<std::string> commandNames() const override { return { "setSoundMode" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _supportedSoundModesMap;
		std::string _soundModeValue;
		std::vector<std::string> _supportedSoundModes;
	};

} // namespace custom
} // namespace smartthings4cpp
