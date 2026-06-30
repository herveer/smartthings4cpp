#include "smartthings4cpp/capabilities/washer_operating_state.h"

#include <utility>

namespace smartthings4cpp {

	WasherOperatingState::WasherOperatingState(int version, std::string componentId,
		std::string deviceId, Client* client)
		: Capability("washerOperatingState", version, std::move(componentId), std::move(deviceId), client) {
	}

	void WasherOperatingState::parseStatus(const nlohmann::json&) {
		std::string machineState = attributeString("machineState", _machineState);
		SetPropertyValueAndNotify<&WasherOperatingState::MachineState>(_machineState, machineState);

		std::string jobState = attributeString("washerJobState", _jobState);
		SetPropertyValueAndNotify<&WasherOperatingState::JobState>(_jobState, jobState);

		std::string completionTime = attributeString("completionTime", _completionTime);
		SetPropertyValueAndNotify<&WasherOperatingState::CompletionTime>(_completionTime, completionTime);

		auto supported = attributeStringArray("supportedMachineStates");
		if (!supported.empty()) {
			SetPropertyValueAndNotify<&WasherOperatingState::SupportedMachineStates>(_supportedMachineStates, supported);
		}
	}

	Result<void> WasherOperatingState::setMachineState(const std::string& state) {
		auto result = executeCommand("setMachineState", nlohmann::json::array({ state }));
		if (result.isSuccess()) {
			setLocalAttribute("machineState", state);
		}
		return result;
	}

} // namespace smartthings4cpp
