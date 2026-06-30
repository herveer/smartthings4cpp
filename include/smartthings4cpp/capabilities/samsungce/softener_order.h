#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.softenerOrder capability (Samsung-proprietary; generated). */
	class SoftenerOrder : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.softenerOrder";

		SoftenerOrder(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<bool> AlarmEnabled{ [this]() { return _alarmEnabled; } };
		ReactiveLitepp::ReadonlyProperty<double> OrderThreshold{ [this]() { return _orderThreshold; } };

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
