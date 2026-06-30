#include "smartthings4cpp/capabilities/media_track_control.h"

#include <utility>

namespace smartthings4cpp {

	MediaTrackControl::MediaTrackControl(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("mediaTrackControl", version, std::move(componentId), std::move(deviceId), client) {
	}

	void MediaTrackControl::parseStatus(const nlohmann::json&) {
		auto commands = attributeStringArray("supportedTrackControlCommands");
		if (!commands.empty()) {
			SetPropertyValueAndNotify<&MediaTrackControl::SupportedCommands>(_supportedCommands, commands);
		}
	}

	Result<void> MediaTrackControl::nextTrack() {
		return executeCommand("nextTrack");
	}

	Result<void> MediaTrackControl::previousTrack() {
		return executeCommand("previousTrack");
	}

} // namespace smartthings4cpp
