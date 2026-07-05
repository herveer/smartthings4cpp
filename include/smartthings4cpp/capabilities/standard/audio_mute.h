#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c audioMute capability (generated). */
	class AudioMute : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "audioMute";

		AudioMute(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> Mute = MakeReadonlyProperty<&AudioMute::Mute>([this]() { return _mute; });

		Result<void> setMute(const std::string& state);
		Result<void> mute();
		Result<void> unmute();
		std::vector<std::string> commandNames() const override { return { "setMute", "mute", "unmute" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _mute;
	};

} // namespace standard
} // namespace smartthings4cpp
