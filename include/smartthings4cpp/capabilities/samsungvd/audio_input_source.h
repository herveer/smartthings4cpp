#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.audioInputSource capability (Samsung-proprietary; generated). */
	class AudioInputSource : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.audioInputSource";

		AudioInputSource(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedInputSources = MakeReadonlyProperty<&AudioInputSource::SupportedInputSources>([this]() { return _supportedInputSources; });
		ReactiveLitepp::ReadonlyProperty<std::string> InputSource = MakeReadonlyProperty<&AudioInputSource::InputSource>([this]() { return _inputSource; });

		Result<void> setNextInputSource();
		std::vector<std::string> commandNames() const override { return { "setNextInputSource" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedInputSources;
		std::string _inputSource;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
