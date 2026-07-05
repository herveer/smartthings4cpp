#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c motionSensor capability (generated). */
	class MotionSensor : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "motionSensor";

		MotionSensor(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> Motion = MakeReadonlyProperty<&MotionSensor::Motion>([this]() { return _motion; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _motion;
	};

} // namespace standard
} // namespace smartthings4cpp
