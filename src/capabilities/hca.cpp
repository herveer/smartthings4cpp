#include "smartthings4cpp/capabilities/hca.h"

#include <utility>

namespace smartthings4cpp {
namespace hca {

	WasherMode::WasherMode(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("hca.washerMode", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherMode::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&WasherMode::Mode>(_mode, attributeString("mode", _mode));
		{ auto v = attributeStringArray("supportedModes"); if (!v.empty()) SetPropertyValueAndNotify<&WasherMode::SupportedModes>(_supportedModes, v); }
	}

	Result<void> WasherMode::setMode(const std::string& mode) {
		nlohmann::json commandArgs = nlohmann::json::array();
		commandArgs.push_back(mode);
		return executeCommand("setMode", commandArgs);
	}

} // namespace hca
} // namespace smartthings4cpp
