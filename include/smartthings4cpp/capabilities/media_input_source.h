#pragma once

#include "../capability.h"
#include <string>
#include <vector>

/**
 * @file media_input_source.h
 * @brief "mediaInputSource" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c mediaInputSource capability: select the active input source
	 *
	 * Attributes: @c inputSource, @c supportedInputSources. Command:
	 * @c setInputSource(mode).
	 */
	class MediaInputSource : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "mediaInputSource";

		MediaInputSource(int version, std::string componentId, std::string deviceId, Client* client);

		/**
		 * @brief Get or set the active input source (e.g. "HDMI1")
		 *
		 * Assigning issues @c setInputSource with the new value.
		 */
		ReactiveLitepp::Property<std::string> InputSource{
			[this]() { return _inputSource; },
			[this](std::string& value) {
				NotifyPropertyChanging<&MediaInputSource::InputSource>();
				setInputSource(value);
			}
		};

		/** @brief Input sources supported by this device (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedInputSources{
			[this]() { return _supportedInputSources; }
		};

		/** @brief Select the input source (@c setInputSource command) */
		Result<void> setInputSource(const std::string& mode);

		std::vector<std::string> commandNames() const override { return { "setInputSource" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _inputSource;
		std::vector<std::string> _supportedInputSources;
	};

} // namespace smartthings4cpp
