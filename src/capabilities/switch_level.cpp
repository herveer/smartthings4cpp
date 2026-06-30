#include "smartthings4cpp/capabilities/switch_level.h"

#include <utility>

namespace smartthings4cpp {

	SwitchLevel::SwitchLevel(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("switchLevel", version, std::move(componentId), std::move(deviceId), client) {
	}

	void SwitchLevel::parseStatus(const nlohmann::json&) {
		int level = static_cast<int>(attributeNumber("level", _level));
		SetPropertyValueAndNotify<&SwitchLevel::Level>(_level, level);

		auto range = attributeValue("levelRange");
		if (range.is_object()) {
			if (range.contains("minimum") && range["minimum"].is_number()) {
				int minimum = range["minimum"].get<int>();
				SetPropertyValueAndNotify<&SwitchLevel::MinLevel>(_minLevel, minimum);
			}
			if (range.contains("maximum") && range["maximum"].is_number()) {
				int maximum = range["maximum"].get<int>();
				SetPropertyValueAndNotify<&SwitchLevel::MaxLevel>(_maxLevel, maximum);
			}
		}
	}

	Result<void> SwitchLevel::setLevel(int level, std::optional<int> rate) {
		nlohmann::json args = nlohmann::json::array();
		args.push_back(level);
		if (rate.has_value()) {
			args.push_back(rate.value());
		}

		auto result = executeCommand("setLevel", args);
		if (result.isSuccess()) {
			setLocalAttribute("level", level);
		}
		return result;
	}

} // namespace smartthings4cpp
