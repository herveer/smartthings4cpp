#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.detergentOrder capability (Samsung-proprietary; generated). */
	class DetergentOrder : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.detergentOrder";

		DetergentOrder(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<bool> AlarmEnabled = MakeReadonlyProperty<&DetergentOrder::AlarmEnabled>([this]() { return _alarmEnabled; });
		ReactiveLitepp::ReadonlyProperty<double> OrderThreshold = MakeReadonlyProperty<&DetergentOrder::OrderThreshold>([this]() { return _orderThreshold; });

		Result<void> enableAlarm();
		Result<void> setOrderThreshold(double threshold);
		Result<void> disableAlarm();
		std::vector<std::string> commandNames() const override { return { "enableAlarm", "setOrderThreshold", "disableAlarm" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		bool _alarmEnabled = false;
		double _orderThreshold = 0.0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
