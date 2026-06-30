#include "smartthings4cpp/capabilities/refresh.h"

#include <utility>

namespace smartthings4cpp {

	Refresh::Refresh(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("refresh", version, std::move(componentId), std::move(deviceId), client) {
	}

	Result<void> Refresh::refresh() {
		return executeCommand("refresh");
	}

} // namespace smartthings4cpp
