#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.autoDispenseDetergent capability (Samsung-proprietary; generated). */
	class AutoDispenseDetergent : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.autoDispenseDetergent";

		AutoDispenseDetergent(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> RemainingAmount{ [this]() { return _remainingAmount; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Amount{ [this]() { return _amount; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedDensity{ [this]() { return _supportedDensity; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Density{ [this]() { return _density; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedAmount{ [this]() { return _supportedAmount; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> AvailableTypes{ [this]() { return _availableTypes; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Type{ [this]() { return _type; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedTypes{ [this]() { return _supportedTypes; } };
		ReactiveLitepp::ReadonlyProperty<int> RecommendedAmount{ [this]() { return _recommendedAmount; } };

		Result<void> unsetRecommendedAmount();
		Result<void> setRecommendedAmount(const std::string& type, int amountPerDose);
		Result<void> setType(const std::string& type);
		Result<void> setAmount(const std::string& amount);
		Result<void> setDensity(const std::string& density);
		std::vector<std::string> commandNames() const override { return { "unsetRecommendedAmount", "setRecommendedAmount", "setType", "setAmount", "setDensity" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _remainingAmount;
		std::string _amount;
		std::vector<std::string> _supportedDensity;
		std::string _density;
		std::vector<std::string> _supportedAmount;
		std::vector<std::string> _availableTypes;
		std::string _type;
		std::vector<std::string> _supportedTypes;
		int _recommendedAmount = 0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
