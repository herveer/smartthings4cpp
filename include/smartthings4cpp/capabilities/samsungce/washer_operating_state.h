#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.washerOperatingState capability (Samsung-proprietary; generated). */
	class WasherOperatingState : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.washerOperatingState";

		WasherOperatingState(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> WashingProgress = MakeReadonlyProperty<&WasherOperatingState::WashingProgress>([this]() { return _washingProgress; });
		ReactiveLitepp::ReadonlyProperty<int> DryingProgress = MakeReadonlyProperty<&WasherOperatingState::DryingProgress>([this]() { return _dryingProgress; });
		ReactiveLitepp::ReadonlyProperty<std::string> WasherJobState = MakeReadonlyProperty<&WasherOperatingState::WasherJobState>([this]() { return _washerJobState; });
		ReactiveLitepp::ReadonlyProperty<std::string> OperatingState = MakeReadonlyProperty<&WasherOperatingState::OperatingState>([this]() { return _operatingState; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedOperatingStates = MakeReadonlyProperty<&WasherOperatingState::SupportedOperatingStates>([this]() { return _supportedOperatingStates; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ScheduledJobs = MakeReadonlyProperty<&WasherOperatingState::ScheduledJobs>([this]() { return _scheduledJobs; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ScheduledPhases = MakeReadonlyProperty<&WasherOperatingState::ScheduledPhases>([this]() { return _scheduledPhases; });
		ReactiveLitepp::ReadonlyProperty<int> Progress = MakeReadonlyProperty<&WasherOperatingState::Progress>([this]() { return _progress; });
		ReactiveLitepp::ReadonlyProperty<std::string> RemainingTimeStr = MakeReadonlyProperty<&WasherOperatingState::RemainingTimeStr>([this]() { return _remainingTimeStr; });
		ReactiveLitepp::ReadonlyProperty<std::string> WasherJobPhase = MakeReadonlyProperty<&WasherOperatingState::WasherJobPhase>([this]() { return _washerJobPhase; });
		ReactiveLitepp::ReadonlyProperty<int> OperationTime = MakeReadonlyProperty<&WasherOperatingState::OperationTime>([this]() { return _operationTime; });
		ReactiveLitepp::ReadonlyProperty<int> RemainingTime = MakeReadonlyProperty<&WasherOperatingState::RemainingTime>([this]() { return _remainingTime; });

		Result<void> resume();
		Result<void> cancel();
		Result<void> start();
		Result<void> estimateOperationTime();
		Result<void> setDelayEnd(int delayTime);
		Result<void> pause();
		std::vector<std::string> commandNames() const override { return { "resume", "cancel", "start", "estimateOperationTime", "setDelayEnd", "pause" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _washingProgress = 0;
		int _dryingProgress = 0;
		std::string _washerJobState;
		std::string _operatingState;
		std::vector<std::string> _supportedOperatingStates;
		nlohmann::json _scheduledJobs;
		nlohmann::json _scheduledPhases;
		int _progress = 0;
		std::string _remainingTimeStr;
		std::string _washerJobPhase;
		int _operationTime = 0;
		int _remainingTime = 0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
