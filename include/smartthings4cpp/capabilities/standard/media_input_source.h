#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c mediaInputSource capability (generated). */
	class MediaInputSource : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "mediaInputSource";

		MediaInputSource(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedInputSources{ [this]() { return _supportedInputSources; } };
		ReactiveLitepp::ReadonlyProperty<std::string> InputSource{ [this]() { return _inputSource; } };

		Result<void> setInputSource(const std::string& mode);
		std::vector<std::string> commandNames() const override { return { "setInputSource" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedInputSources;
		std::string _inputSource;
	};

} // namespace standard
} // namespace smartthings4cpp
