#pragma once

#include "../capability.h"
#include <string>
#include <vector>

/**
 * @file media_track_control.h
 * @brief "mediaTrackControl" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c mediaTrackControl capability: skip between tracks
	 *
	 * Attribute: @c supportedTrackControlCommands. Commands: @c nextTrack,
	 * @c previousTrack.
	 */
	class MediaTrackControl : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "mediaTrackControl";

		MediaTrackControl(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief Track-control commands supported by this device (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedCommands{
			[this]() { return _supportedCommands; }
		};

		Result<void> nextTrack();
		Result<void> previousTrack();

		std::vector<std::string> commandNames() const override { return { "nextTrack", "previousTrack" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedCommands;
	};

} // namespace smartthings4cpp
