#pragma once

#include "../capability.h"
#include <string>
#include <vector>

/**
 * @file media_playback.h
 * @brief "mediaPlayback" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c mediaPlayback capability: transport control for media
	 *
	 * Attributes: @c playbackStatus, @c supportedPlaybackCommands. Commands:
	 * @c play, @c pause, @c stop, @c fastForward, @c rewind, @c setPlaybackStatus.
	 */
	class MediaPlayback : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "mediaPlayback";

		MediaPlayback(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief Current playback status, e.g. "playing"/"paused"/"stopped" (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> PlaybackStatus{
			[this]() { return _playbackStatus; }
		};

		/** @brief Playback commands supported by this device (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedCommands{
			[this]() { return _supportedCommands; }
		};

		Result<void> play();
		Result<void> pause();
		Result<void> stop();
		Result<void> fastForward();
		Result<void> rewind();
		/** @brief Set the playback status directly (@c setPlaybackStatus command) */
		Result<void> setPlaybackStatus(const std::string& status);

		std::vector<std::string> commandNames() const override {
			return { "play", "pause", "stop", "fastForward", "rewind", "setPlaybackStatus" };
		}

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _playbackStatus;
		std::vector<std::string> _supportedCommands;
	};

} // namespace smartthings4cpp
