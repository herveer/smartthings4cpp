#include "smartthings4cpp/capabilities/audio_volume.h"

#include <utility>

namespace smartthings4cpp {

	AudioVolume::AudioVolume(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("audioVolume", version, std::move(componentId), std::move(deviceId), client) {
	}

	void AudioVolume::parseStatus(const nlohmann::json&) {
		int volume = static_cast<int>(attributeNumber("volume", _volume));
		SetPropertyValueAndNotify<&AudioVolume::Volume>(_volume, volume);
	}

	Result<void> AudioVolume::setVolume(int volume) {
		nlohmann::json args = nlohmann::json::array();
		args.push_back(volume);

		auto result = executeCommand("setVolume", args);
		if (result.isSuccess()) {
			setLocalAttribute("volume", volume);
		}
		return result;
	}

	Result<void> AudioVolume::volumeUp() {
		return executeCommand("volumeUp");
	}

	Result<void> AudioVolume::volumeDown() {
		return executeCommand("volumeDown");
	}

} // namespace smartthings4cpp
