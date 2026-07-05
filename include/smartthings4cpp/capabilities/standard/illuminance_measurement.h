#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c illuminanceMeasurement capability (generated). */
	class IlluminanceMeasurement : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "illuminanceMeasurement";

		IlluminanceMeasurement(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<double> Illuminance = MakeReadonlyProperty<&IlluminanceMeasurement::Illuminance>([this]() { return _illuminance; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		double _illuminance = 0.0;
	};

} // namespace standard
} // namespace smartthings4cpp
