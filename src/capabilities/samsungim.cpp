#include "smartthings4cpp/capabilities/samsungim.h"

#include <utility>

namespace smartthings4cpp {
namespace samsungim {

	FixedFindNode::FixedFindNode(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungim.fixedFindNode", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> FixedFindNode::refresh() {
		return executeCommand("refresh");
	}

	HueSyncMode::HueSyncMode(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("samsungim.hueSyncMode", version, std::move(componentId), std::move(deviceId), client) {
	}

	void HueSyncMode::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&HueSyncMode::Mode>(_mode, attributeString("mode", _mode));
	}

} // namespace samsungim
} // namespace smartthings4cpp
