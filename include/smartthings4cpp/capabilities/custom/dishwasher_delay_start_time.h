#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.dishwasherDelayStartTime capability (Samsung-proprietary; generated). */
	class DishwasherDelayStartTime : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.dishwasherDelayStartTime";

		DishwasherDelayStartTime(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> DishwasherDelayStartTimeValue = MakeReadonlyProperty<&DishwasherDelayStartTime::DishwasherDelayStartTimeValue>([this]() { return _dishwasherDelayStartTimeValue; });

		Result<void> setDishwasherDelayStartTime(const std::string& dishwasherDelayStartTime);
		std::vector<std::string> commandNames() const override { return { "setDishwasherDelayStartTime" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _dishwasherDelayStartTimeValue;
	};

} // namespace custom
} // namespace smartthings4cpp
