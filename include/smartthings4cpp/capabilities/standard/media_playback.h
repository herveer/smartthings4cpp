#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c mediaPlayback capability (generated). */
	class MediaPlayback : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "mediaPlayback";

		MediaPlayback(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedPlaybackCommands{ [this]() { return _supportedPlaybackCommands; } };
		ReactiveLitepp::ReadonlyProperty<std::string> PlaybackStatus{ [this]() { return _playbackStatus; } };

		Result<void> play();
		Result<void> stop();
		Result<void> rewind();
		Result<void> fastForward();
		Result<void> setPlaybackStatus(const std::string& status);
		Result<void> pause();
		std::vector<std::string> commandNames() const override { return { "play", "stop", "rewind", "fastForward", "setPlaybackStatus", "pause" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedPlaybackCommands;
		std::string _playbackStatus;
	};

} // namespace standard
} // namespace smartthings4cpp
