#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.dishwasherOperatingPercentage capability (Samsung-proprietary; generated). */
	class DishwasherOperatingPercentage : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.dishwasherOperatingPercentage";

		DishwasherOperatingPercentage(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> DishwasherOperatingPercentageValue{ [this]() { return _dishwasherOperatingPercentageValue; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _dishwasherOperatingPercentageValue = 0;
	};

} // namespace custom
} // namespace smartthings4cpp
