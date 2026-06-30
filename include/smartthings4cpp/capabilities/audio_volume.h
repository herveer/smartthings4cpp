#pragma once

#include "../capability.h"

/**
 * @file audio_volume.h
 * @brief "audioVolume" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c audioVolume capability: volume 0-100%
	 *
	 * Attribute: @c volume (integer %). Commands: @c setVolume(volume),
	 * @c volumeUp, @c volumeDown.
	 */
	class AudioVolume : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "audioVolume";

		AudioVolume(int version, std::string componentId, std::string deviceId, Client* client);

		/**
		 * @brief Get or set the volume, 0-100
		 *
		 * Assigning issues @c setVolume with the new value.
		 */
		ReactiveLitepp::Property<int> Volume{
			[this]() { return _volume; },
			[this](int& value) {
				NotifyPropertyChanging<&AudioVolume::Volume>();
				setVolume(value);
			}
		};

		/** @brief Set the volume (@c setVolume command) */
		Result<void> setVolume(int volume);
		/** @brief Increase the volume by one step (@c volumeUp command) */
		Result<void> volumeUp();
		/** @brief Decrease the volume by one step (@c volumeDown command) */
		Result<void> volumeDown();

		std::vector<std::string> commandNames() const override { return { "setVolume", "volumeUp", "volumeDown" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _volume = 0;
	};

} // namespace smartthings4cpp
