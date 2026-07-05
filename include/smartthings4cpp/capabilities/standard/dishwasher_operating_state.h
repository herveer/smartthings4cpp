#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c dishwasherOperatingState capability (generated). */
	class DishwasherOperatingState : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "dishwasherOperatingState";

		DishwasherOperatingState(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> CompletionTime{ [this]() { return _completionTime; } };
		ReactiveLitepp::ReadonlyProperty<std::string> MachineState{ [this]() { return _machineState; } };
		ReactiveLitepp::ReadonlyProperty<int> Progress{ [this]() { return _progress; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedMachineStates{ [this]() { return _supportedMachineStates; } };
		ReactiveLitepp::ReadonlyProperty<std::string> DishwasherJobState{ [this]() { return _dishwasherJobState; } };

		Result<void> setMachineState(const std::string& state);
		std::vector<std::string> commandNames() const override { return { "setMachineState" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _completionTime;
		std::string _machineState;
		int _progress = 0;
		std::vector<std::string> _supportedMachineStates;
		std::string _dishwasherJobState;
	};

} // namespace standard
} // namespace smartthings4cpp
