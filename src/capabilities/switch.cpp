#include "smartthings4cpp/capabilities/switch.h"

#include <utility>

namespace smartthings4cpp {

	Switch::Switch(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("switch", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Switch::parseStatus(const nlohmann::json&) {
		std::string value = attributeString("switch", _state);
		if (SetPropertyValueAndNotify<&Switch::State>(_state, value)) {
			NotifyPropertyChanged<&Switch::IsOn>();
			NotifyPropertyChanged<&Switch::On>();
		}
	}

	Result<void> Switch::on() {
		auto result = executeCommand("on");
		if (result.isSuccess()) {
			setLocalAttribute("switch", "on");
		}
		return result;
	}

	Result<void> Switch::off() {
		auto result = executeCommand("off");
		if (result.isSuccess()) {
			setLocalAttribute("switch", "off");
		}
		return result;
	}

	Result<void> Switch::toggle() {
		return (_state == "on") ? off() : on();
	}

} // namespace smartthings4cpp
