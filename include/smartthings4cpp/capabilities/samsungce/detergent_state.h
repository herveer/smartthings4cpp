#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.detergentState capability (Samsung-proprietary; generated). */
	class DetergentState : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.detergentState";

		DetergentState(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<double> RemainingAmount = MakeReadonlyProperty<&DetergentState::RemainingAmount>([this]() { return _remainingAmount; });
		ReactiveLitepp::ReadonlyProperty<double> Dosage = MakeReadonlyProperty<&DetergentState::Dosage>([this]() { return _dosage; });
		ReactiveLitepp::ReadonlyProperty<double> InitialAmount = MakeReadonlyProperty<&DetergentState::InitialAmount>([this]() { return _initialAmount; });
		ReactiveLitepp::ReadonlyProperty<std::string> DetergentType = MakeReadonlyProperty<&DetergentState::DetergentType>([this]() { return _detergentType; });

		Result<void> setDetergentType(const std::string& detergentType);
		Result<void> setInitialAmount(double amount);
		Result<void> setRemainingAmount(double amount);
		Result<void> setDosage(double amount);
		std::vector<std::string> commandNames() const override { return { "setDetergentType", "setInitialAmount", "setRemainingAmount", "setDosage" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		double _remainingAmount = 0.0;
		double _dosage = 0.0;
		double _initialAmount = 0.0;
		std::string _detergentType;
	};

} // namespace samsungce
} // namespace smartthings4cpp
