#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.foodDefrost capability (Samsung-proprietary; generated). */
	class FoodDefrost : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.foodDefrost";

		FoodDefrost(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedOptions = MakeReadonlyProperty<&FoodDefrost::SupportedOptions>([this]() { return _supportedOptions; });
		ReactiveLitepp::ReadonlyProperty<std::string> FoodType = MakeReadonlyProperty<&FoodDefrost::FoodType>([this]() { return _foodType; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Weight = MakeReadonlyProperty<&FoodDefrost::Weight>([this]() { return _weight; });
		ReactiveLitepp::ReadonlyProperty<int> OperationTime = MakeReadonlyProperty<&FoodDefrost::OperationTime>([this]() { return _operationTime; });
		ReactiveLitepp::ReadonlyProperty<int> RemainingTime = MakeReadonlyProperty<&FoodDefrost::RemainingTime>([this]() { return _remainingTime; });

		Result<void> setDefrost(const std::string& foodType, const nlohmann::json& weight);
		std::vector<std::string> commandNames() const override { return { "setDefrost" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _supportedOptions;
		std::string _foodType;
		nlohmann::json _weight;
		int _operationTime = 0;
		int _remainingTime = 0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
