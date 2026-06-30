#include "smartthings4cpp/capabilities/audio_mute.h"

#include <utility>

namespace smartthings4cpp {

	AudioMute::AudioMute(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("audioMute", version, std::move(componentId), std::move(deviceId), client) {
	}

	void AudioMute::parseStatus(const nlohmann::json&) {
		std::string value = attributeString("mute", _mute);
		if (SetPropertyValueAndNotify<&AudioMute::MuteState>(_mute, value)) {
			NotifyPropertyChanged<&AudioMute::IsMuted>();
			NotifyPropertyChanged<&AudioMute::Muted>();
		}
	}

	Result<void> AudioMute::mute() {
		auto result = executeCommand("mute");
		if (result.isSuccess()) {
			setLocalAttribute("mute", "muted");
		}
		return result;
	}

	Result<void> AudioMute::unmute() {
		auto result = executeCommand("unmute");
		if (result.isSuccess()) {
			setLocalAttribute("mute", "unmuted");
		}
		return result;
	}

	Result<void> AudioMute::setMute(const std::string& state) {
		nlohmann::json args = nlohmann::json::array();
		args.push_back(state);

		auto result = executeCommand("setMute", args);
		if (result.isSuccess()) {
			setLocalAttribute("mute", state);
		}
		return result;
	}

} // namespace smartthings4cpp
