#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.autoDispenseSoftener capability (Samsung-proprietary; generated). */
	class AutoDispenseSoftener : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.autoDispenseSoftener";

		AutoDispenseSoftener(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> RemainingAmount{ [this]() { return _remainingAmount; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Amount{ [this]() { return _amount; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedDensity{ [this]() { return _supportedDensity; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Density{ [this]() { return _density; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedAmount{ [this]() { return _supportedAmount; } };

		Result<void> setAmount(const std::string& amount);
		Result<void> setDensity(const std::string& density);
		std::vector<std::string> commandNames() const override { return { "setAmount", "setDensity" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _remainingAmount;
		std::string _amount;
		std::vector<std::string> _supportedDensity;
		std::string _density;
		std::vector<std::string> _supportedAmount;
	};

} // namespace samsungce
} // namespace smartthings4cpp
