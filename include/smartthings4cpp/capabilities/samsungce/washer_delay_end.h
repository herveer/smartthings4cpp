#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.washerDelayEnd capability (Samsung-proprietary; generated). */
	class WasherDelayEnd : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.washerDelayEnd";

		WasherDelayEnd(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> RemainingTime = MakeReadonlyProperty<&WasherDelayEnd::RemainingTime>([this]() { return _remainingTime; });
		ReactiveLitepp::ReadonlyProperty<int> MinimumReservableTime = MakeReadonlyProperty<&WasherDelayEnd::MinimumReservableTime>([this]() { return _minimumReservableTime; });

		Result<void> refreshMinimumReservableTime();
		Result<void> setDelayTime(int delayTime);
		std::vector<std::string> commandNames() const override { return { "refreshMinimumReservableTime", "setDelayTime" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _remainingTime = 0;
		int _minimumReservableTime = 0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
