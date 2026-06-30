#pragma once

#include "../capability.h"
#include <string>
#include <vector>

/**
 * @file washer_operating_state.h
 * @brief "washerOperatingState" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c washerOperatingState capability: washer run state
	 *
	 * Attributes: @c machineState ("pause"/"run"/"stop"), @c washerJobState,
	 * @c completionTime, @c supportedMachineStates. Command: @c setMachineState.
	 */
	class WasherOperatingState : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "washerOperatingState";

		WasherOperatingState(int version, std::string componentId, std::string deviceId, Client* client);

		/**
		 * @brief Get or set the machine state ("pause"/"run"/"stop")
		 *
		 * Assigning issues @c setMachineState with the new value.
		 */
		ReactiveLitepp::Property<std::string> MachineState{
			[this]() { return _machineState; },
			[this](std::string& value) {
				NotifyPropertyChanging<&WasherOperatingState::MachineState>();
				setMachineState(value);
			}
		};

		/** @brief The current washer job state (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> JobState{
			[this]() { return _jobState; }
		};

		/** @brief Estimated completion time, ISO-8601 (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> CompletionTime{
			[this]() { return _completionTime; }
		};

		/** @brief Machine states supported by this device (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedMachineStates{
			[this]() { return _supportedMachineStates; }
		};

		/** @brief Set the machine state (@c setMachineState command) */
		Result<void> setMachineState(const std::string& state);

		std::vector<std::string> commandNames() const override { return { "setMachineState" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _machineState;
		std::string _jobState;
		std::string _completionTime;
		std::vector<std::string> _supportedMachineStates;
	};

} // namespace smartthings4cpp
