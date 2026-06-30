#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.detergentAutoReplenishment capability (Samsung-proprietary; generated). */
	class DetergentAutoReplenishment : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.detergentAutoReplenishment";

		DetergentAutoReplenishment(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> NeutralDetergentType{ [this]() { return _neutralDetergentType; } };
		ReactiveLitepp::ReadonlyProperty<double> RegularDetergentRemainingAmount{ [this]() { return _regularDetergentRemainingAmount; } };
		ReactiveLitepp::ReadonlyProperty<double> BabyDetergentRemainingAmount{ [this]() { return _babyDetergentRemainingAmount; } };
		ReactiveLitepp::ReadonlyProperty<double> NeutralDetergentRemainingAmount{ [this]() { return _neutralDetergentRemainingAmount; } };
		ReactiveLitepp::ReadonlyProperty<bool> NeutralDetergentAlarmEnabled{ [this]() { return _neutralDetergentAlarmEnabled; } };
		ReactiveLitepp::ReadonlyProperty<double> NeutralDetergentOrderThreshold{ [this]() { return _neutralDetergentOrderThreshold; } };
		ReactiveLitepp::ReadonlyProperty<double> BabyDetergentInitialAmount{ [this]() { return _babyDetergentInitialAmount; } };
		ReactiveLitepp::ReadonlyProperty<std::string> BabyDetergentType{ [this]() { return _babyDetergentType; } };
		ReactiveLitepp::ReadonlyProperty<double> NeutralDetergentInitialAmount{ [this]() { return _neutralDetergentInitialAmount; } };
		ReactiveLitepp::ReadonlyProperty<double> RegularDetergentDosage{ [this]() { return _regularDetergentDosage; } };
		ReactiveLitepp::ReadonlyProperty<double> BabyDetergentDosage{ [this]() { return _babyDetergentDosage; } };
		ReactiveLitepp::ReadonlyProperty<double> RegularDetergentOrderThreshold{ [this]() { return _regularDetergentOrderThreshold; } };
		ReactiveLitepp::ReadonlyProperty<std::string> RegularDetergentType{ [this]() { return _regularDetergentType; } };
		ReactiveLitepp::ReadonlyProperty<double> RegularDetergentInitialAmount{ [this]() { return _regularDetergentInitialAmount; } };
		ReactiveLitepp::ReadonlyProperty<bool> RegularDetergentAlarmEnabled{ [this]() { return _regularDetergentAlarmEnabled; } };
		ReactiveLitepp::ReadonlyProperty<double> NeutralDetergentDosage{ [this]() { return _neutralDetergentDosage; } };
		ReactiveLitepp::ReadonlyProperty<double> BabyDetergentOrderThreshold{ [this]() { return _babyDetergentOrderThreshold; } };
		ReactiveLitepp::ReadonlyProperty<bool> BabyDetergentAlarmEnabled{ [this]() { return _babyDetergentAlarmEnabled; } };

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
		std::string _neutralDetergentType;
		double _regularDetergentRemainingAmount = 0.0;
		double _babyDetergentRemainingAmount = 0.0;
		double _neutralDetergentRemainingAmount = 0.0;
		bool _neutralDetergentAlarmEnabled = false;
		double _neutralDetergentOrderThreshold = 0.0;
		double _babyDetergentInitialAmount = 0.0;
		std::string _babyDetergentType;
		double _neutralDetergentInitialAmount = 0.0;
		double _regularDetergentDosage = 0.0;
		double _babyDetergentDosage = 0.0;
		double _regularDetergentOrderThreshold = 0.0;
		std::string _regularDetergentType;
		double _regularDetergentInitialAmount = 0.0;
		bool _regularDetergentAlarmEnabled = false;
		double _neutralDetergentDosage = 0.0;
		double _babyDetergentOrderThreshold = 0.0;
		bool _babyDetergentAlarmEnabled = false;
	};

} // namespace samsungce
} // namespace smartthings4cpp
