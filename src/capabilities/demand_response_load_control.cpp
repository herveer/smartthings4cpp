#include "smartthings4cpp/capabilities/demand_response_load_control.h"

#include <utility>

namespace smartthings4cpp {

	DemandResponseLoadControl::DemandResponseLoadControl(int version, std::string componentId,
		std::string deviceId, Client* client)
		: Capability("demandResponseLoadControl", version, std::move(componentId), std::move(deviceId), client) {
	}

	void DemandResponseLoadControl::parseStatus(const nlohmann::json&) {
		auto value = attributeValue("drlcStatus");
		if (!value.is_object()) {
			return;
		}

		auto integer = [&](const char* key, int fallback) {
			return (value.contains(key) && value[key].is_number_integer())
				? value[key].get<int>() : fallback;
		};

		SetPropertyValueAndNotify<&DemandResponseLoadControl::DrlcType>(_drlcType, integer("drlcType", _drlcType));
		SetPropertyValueAndNotify<&DemandResponseLoadControl::DrlcLevel>(_drlcLevel, integer("drlcLevel", _drlcLevel));
		SetPropertyValueAndNotify<&DemandResponseLoadControl::Duration>(_duration, integer("duration", _duration));

		bool overrideValue = (value.contains("override") && value["override"].is_boolean())
			? value["override"].get<bool>() : _override;
		SetPropertyValueAndNotify<&DemandResponseLoadControl::Override>(_override, overrideValue);

		std::string start = (value.contains("start") && value["start"].is_string())
			? value["start"].get<std::string>() : _start;
		SetPropertyValueAndNotify<&DemandResponseLoadControl::Start>(_start, start);
	}

	Result<void> DemandResponseLoadControl::overrideDrlcAction(bool value) {
		return executeCommand("overrideDrlcAction", nlohmann::json::array({ value }));
	}

	Result<void> DemandResponseLoadControl::requestDrlcAction(int drlcType, int drlcLevel,
		const std::string& start, int duration, std::optional<int> reportingPeriod) {
		nlohmann::json args = nlohmann::json::array({ drlcType, drlcLevel, start, duration });
		if (reportingPeriod.has_value()) {
			args.push_back(reportingPeriod.value());
		}
		return executeCommand("requestDrlcAction", args);
	}

} // namespace smartthings4cpp
