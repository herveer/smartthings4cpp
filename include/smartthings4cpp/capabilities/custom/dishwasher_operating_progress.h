#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.dishwasherOperatingProgress capability (Samsung-proprietary; generated). */
	class DishwasherOperatingProgress : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.dishwasherOperatingProgress";

		DishwasherOperatingProgress(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> DishwasherOperatingProgressValue{ [this]() { return _dishwasherOperatingProgress; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _dishwasherOperatingProgress;
	};

} // namespace custom
} // namespace smartthings4cpp
