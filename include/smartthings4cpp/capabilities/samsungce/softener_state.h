#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.softenerState capability (Samsung-proprietary; generated). */
	class SoftenerState : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.softenerState";

		SoftenerState(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<double> RemainingAmount{ [this]() { return _remainingAmount; } };
		ReactiveLitepp::ReadonlyProperty<double> Dosage{ [this]() { return _dosage; } };
		ReactiveLitepp::ReadonlyProperty<std::string> SoftenerType{ [this]() { return _softenerType; } };
		ReactiveLitepp::ReadonlyProperty<double> InitialAmount{ [this]() { return _initialAmount; } };

		Result<void> setInitialAmount(double amount);
		Result<void> setRemainingAmount(double amount);
		Result<void> setDosage(double amount);
		Result<void> setSoftenerType(const std::string& softenerType);
		std::vector<std::string> commandNames() const override { return { "setInitialAmount", "setRemainingAmount", "setDosage", "setSoftenerType" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		double _remainingAmount = 0.0;
		double _dosage = 0.0;
		std::string _softenerType;
		double _initialAmount = 0.0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
