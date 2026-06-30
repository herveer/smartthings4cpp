#include "smartthings4cpp/capabilities/unknown_capability.h"

#include <utility>

namespace smartthings4cpp {

	UnknownCapability::UnknownCapability(std::string id, int version, std::string componentId,
		std::string deviceId, Client* client)
		: Capability(std::move(id), version, std::move(componentId), std::move(deviceId), client) {
	}

} // namespace smartthings4cpp
