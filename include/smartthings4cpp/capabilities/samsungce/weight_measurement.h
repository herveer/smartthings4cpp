#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.weightMeasurement capability (Samsung-proprietary; generated). */
	class WeightMeasurement : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.weightMeasurement";

		WeightMeasurement(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<double> Weight = MakeReadonlyProperty<&WeightMeasurement::Weight>([this]() { return _weight; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		double _weight = 0.0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
