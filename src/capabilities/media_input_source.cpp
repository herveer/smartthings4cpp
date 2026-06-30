#include "smartthings4cpp/capabilities/media_input_source.h"

#include <utility>

namespace smartthings4cpp {

	MediaInputSource::MediaInputSource(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("mediaInputSource", version, std::move(componentId), std::move(deviceId), client) {
	}

	void MediaInputSource::parseStatus(const nlohmann::json&) {
		std::string source = attributeString("inputSource", _inputSource);
		SetPropertyValueAndNotify<&MediaInputSource::InputSource>(_inputSource, source);

		auto sources = attributeStringArray("supportedInputSources");
		if (!sources.empty()) {
			SetPropertyValueAndNotify<&MediaInputSource::SupportedInputSources>(_supportedInputSources, sources);
		}
	}

	Result<void> MediaInputSource::setInputSource(const std::string& mode) {
		auto result = executeCommand("setInputSource", nlohmann::json::array({ mode }));
		if (result.isSuccess()) {
			setLocalAttribute("inputSource", mode);
		}
		return result;
	}

} // namespace smartthings4cpp
