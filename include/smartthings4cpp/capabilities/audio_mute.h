#pragma once

#include "../capability.h"

/**
 * @file audio_mute.h
 * @brief "audioMute" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c audioMute capability: mute / unmute audio
	 *
	 * Attribute: @c mute ("muted"/"unmuted"). Commands: @c setMute(state),
	 * @c mute, @c unmute.
	 */
	class AudioMute : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "audioMute";

		AudioMute(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief Raw mute state, "muted" or "unmuted" (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> MuteState{
			[this]() { return _mute; }
		};

		/** @brief Whether audio is muted (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<bool> IsMuted{
			[this]() { return _mute == "muted"; }
		};

		/**
		 * @brief Get or set the muted state
		 *
		 * Assigning issues the @c mute or @c unmute command.
		 */
		ReactiveLitepp::Property<bool> Muted{
			[this]() { return _mute == "muted"; },
			[this](bool& value) {
				NotifyPropertyChanging<&AudioMute::Muted>();
				value ? mute() : unmute();
			}
		};

		/** @brief Mute the audio (@c mute command) */
		Result<void> mute();
		/** @brief Unmute the audio (@c unmute command) */
		Result<void> unmute();
		/** @brief Set the mute state (@c setMute command); @p state is "muted" or "unmuted" */
		Result<void> setMute(const std::string& state);

		std::vector<std::string> commandNames() const override { return { "setMute", "mute", "unmute" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _mute = "unmuted";
	};

} // namespace smartthings4cpp
