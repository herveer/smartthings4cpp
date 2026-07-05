#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c audioNotification capability (generated). */
	class AudioNotification : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "audioNotification";

		AudioNotification(int version, std::string componentId, std::string deviceId, Client* client);

		Result<void> playTrackAndResume(const std::string& uri, int level);
		Result<void> playTrackAndRestore(const std::string& uri, int level);
		Result<void> playTrack(const std::string& uri, int level);
		std::vector<std::string> commandNames() const override { return { "playTrackAndResume", "playTrackAndRestore", "playTrack" }; }
	};

} // namespace standard
} // namespace smartthings4cpp
