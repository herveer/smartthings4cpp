#include "smartthings4cpp/capabilities/remote_control_status.h"

#include <utility>

namespace smartthings4cpp {

	RemoteControlStatus::RemoteControlStatus(int version, std::string componentId,
		std::string deviceId, Client* client)
		: Capability("remoteControlStatus", version, std::move(componentId), std::move(deviceId), client) {
	}

	void RemoteControlStatus::parseStatus(const nlohmann::json&) {
		std::string value = attributeString("remoteControlEnabled", _enabled);
		if (SetPropertyValueAndNotify<&RemoteControlStatus::RemoteControlEnabled>(_enabled, value)) {
			NotifyPropertyChanged<&RemoteControlStatus::Enabled>();
		}
	}

} // namespace smartthings4cpp
