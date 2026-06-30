#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.dishwasherWashingCourseDetails capability (Samsung-proprietary; generated). */
	class DishwasherWashingCourseDetails : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.dishwasherWashingCourseDetails";

		DishwasherWashingCourseDetails(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> PredefinedCourses{ [this]() { return _predefinedCourses; } };
		ReactiveLitepp::ReadonlyProperty<int> WaterUsageMax{ [this]() { return _waterUsageMax; } };
		ReactiveLitepp::ReadonlyProperty<int> EnergyUsageMax{ [this]() { return _energyUsageMax; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _predefinedCourses;
		int _waterUsageMax = 0;
		int _energyUsageMax = 0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
