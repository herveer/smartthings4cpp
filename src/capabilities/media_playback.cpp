#include "smartthings4cpp/capabilities/media_playback.h"

#include <utility>

namespace smartthings4cpp {

	MediaPlayback::MediaPlayback(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("mediaPlayback", version, std::move(componentId), std::move(deviceId), client) {
	}

	void MediaPlayback::parseStatus(const nlohmann::json&) {
		std::string status = attributeString("playbackStatus", _playbackStatus);
		SetPropertyValueAndNotify<&MediaPlayback::PlaybackStatus>(_playbackStatus, status);

		auto commands = attributeStringArray("supportedPlaybackCommands");
		if (!commands.empty()) {
			SetPropertyValueAndNotify<&MediaPlayback::SupportedCommands>(_supportedCommands, commands);
		}
	}

	Result<void> MediaPlayback::play() {
		auto result = executeCommand("play");
		if (result.isSuccess()) {
			setLocalAttribute("playbackStatus", "playing");
		}
		return result;
	}

	Result<void> MediaPlayback::pause() {
		auto result = executeCommand("pause");
		if (result.isSuccess()) {
			setLocalAttribute("playbackStatus", "paused");
		}
		return result;
	}

	Result<void> MediaPlayback::stop() {
		auto result = executeCommand("stop");
		if (result.isSuccess()) {
			setLocalAttribute("playbackStatus", "stopped");
		}
		return result;
	}

	Result<void> MediaPlayback::fastForward() {
		auto result = executeCommand("fastForward");
		if (result.isSuccess()) {
			setLocalAttribute("playbackStatus", "fast forwarding");
		}
		return result;
	}

	Result<void> MediaPlayback::rewind() {
		auto result = executeCommand("rewind");
		if (result.isSuccess()) {
			setLocalAttribute("playbackStatus", "rewinding");
		}
		return result;
	}

	Result<void> MediaPlayback::setPlaybackStatus(const std::string& status) {
		auto result = executeCommand("setPlaybackStatus", nlohmann::json::array({ status }));
		if (result.isSuccess()) {
			setLocalAttribute("playbackStatus", status);
		}
		return result;
	}

} // namespace smartthings4cpp
