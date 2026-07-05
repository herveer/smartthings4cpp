#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c washerOperatingState capability (generated). */
	class WasherOperatingState : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "washerOperatingState";

		WasherOperatingState(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> CompletionTime = MakeReadonlyProperty<&WasherOperatingState::CompletionTime>([this]() { return _completionTime; });
		ReactiveLitepp::ReadonlyProperty<std::string> MachineState = MakeReadonlyProperty<&WasherOperatingState::MachineState>([this]() { return _machineState; });
		ReactiveLitepp::ReadonlyProperty<std::string> WasherJobState = MakeReadonlyProperty<&WasherOperatingState::WasherJobState>([this]() { return _washerJobState; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedMachineStates = MakeReadonlyProperty<&WasherOperatingState::SupportedMachineStates>([this]() { return _supportedMachineStates; });

		Result<void> setMachineState(const std::string& state);
		std::vector<std::string> commandNames() const override { return { "setMachineState" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _completionTime;
		std::string _machineState;
		std::string _washerJobState;
		std::vector<std::string> _supportedMachineStates;
	};

} // namespace standard
} // namespace smartthings4cpp
