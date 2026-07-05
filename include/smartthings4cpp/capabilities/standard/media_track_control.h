#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c mediaTrackControl capability (generated). */
	class MediaTrackControl : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "mediaTrackControl";

		MediaTrackControl(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedTrackControlCommands = MakeReadonlyProperty<&MediaTrackControl::SupportedTrackControlCommands>([this]() { return _supportedTrackControlCommands; });

		Result<void> previousTrack();
		Result<void> nextTrack();
		std::vector<std::string> commandNames() const override { return { "previousTrack", "nextTrack" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedTrackControlCommands;
	};

} // namespace standard
} // namespace smartthings4cpp
