#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c soundDetection capability (generated). */
	class SoundDetection : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "soundDetection";

		SoundDetection(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> SoundDetectionState{ [this]() { return _soundDetectionState; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedSoundTypes{ [this]() { return _supportedSoundTypes; } };
		ReactiveLitepp::ReadonlyProperty<std::string> SoundDetected{ [this]() { return _soundDetected; } };

		Result<void> disableSoundDetection();
		Result<void> enableSoundDetection();
		std::vector<std::string> commandNames() const override { return { "disableSoundDetection", "enableSoundDetection" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _soundDetectionState;
		std::vector<std::string> _supportedSoundTypes;
		std::string _soundDetected;
	};

} // namespace standard
} // namespace smartthings4cpp
