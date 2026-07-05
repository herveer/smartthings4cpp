#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.softenerAutoReplenishment capability (Samsung-proprietary; generated). */
	class SoftenerAutoReplenishment : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.softenerAutoReplenishment";

		SoftenerAutoReplenishment(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> RegularSoftenerType = MakeReadonlyProperty<&SoftenerAutoReplenishment::RegularSoftenerType>([this]() { return _regularSoftenerType; });
		ReactiveLitepp::ReadonlyProperty<bool> RegularSoftenerAlarmEnabled = MakeReadonlyProperty<&SoftenerAutoReplenishment::RegularSoftenerAlarmEnabled>([this]() { return _regularSoftenerAlarmEnabled; });
		ReactiveLitepp::ReadonlyProperty<double> RegularSoftenerInitialAmount = MakeReadonlyProperty<&SoftenerAutoReplenishment::RegularSoftenerInitialAmount>([this]() { return _regularSoftenerInitialAmount; });
		ReactiveLitepp::ReadonlyProperty<double> RegularSoftenerRemainingAmount = MakeReadonlyProperty<&SoftenerAutoReplenishment::RegularSoftenerRemainingAmount>([this]() { return _regularSoftenerRemainingAmount; });
		ReactiveLitepp::ReadonlyProperty<double> RegularSoftenerDosage = MakeReadonlyProperty<&SoftenerAutoReplenishment::RegularSoftenerDosage>([this]() { return _regularSoftenerDosage; });
		ReactiveLitepp::ReadonlyProperty<double> RegularSoftenerOrderThreshold = MakeReadonlyProperty<&SoftenerAutoReplenishment::RegularSoftenerOrderThreshold>([this]() { return _regularSoftenerOrderThreshold; });

		Result<void> enableAlarm(const std::string& name);
		Result<void> setOrderThreshold(const std::string& name, double threshold);
		Result<void> disableAlarm(const std::string& name);
		Result<void> setAutoReplenishment(const nlohmann::json& autoReplenishment);
		Result<void> setInitialAmount(const std::string& name, double amount);
		Result<void> setRemainingAmount(const std::string& name, double amount);
		Result<void> setDosage(const std::string& name, double amount);
		Result<void> setType(const std::string& name, const std::string& type);
		std::vector<std::string> commandNames() const override { return { "enableAlarm", "setOrderThreshold", "disableAlarm", "setAutoReplenishment", "setInitialAmount", "setRemainingAmount", "setDosage", "setType" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _regularSoftenerType;
		bool _regularSoftenerAlarmEnabled = false;
		double _regularSoftenerInitialAmount = 0.0;
		double _regularSoftenerRemainingAmount = 0.0;
		double _regularSoftenerDosage = 0.0;
		double _regularSoftenerOrderThreshold = 0.0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
