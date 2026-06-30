#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.weightMeasurementCalibration capability (Samsung-proprietary; generated). */
	class WeightMeasurementCalibration : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.weightMeasurementCalibration";

		WeightMeasurementCalibration(int version, std::string componentId, std::string deviceId, Client* client);

		Result<void> zeroCalibrate();
		std::vector<std::string> commandNames() const override { return { "zeroCalibrate" }; }
	};

} // namespace samsungce
} // namespace smartthings4cpp
