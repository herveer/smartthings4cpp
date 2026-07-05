#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c movementSensor capability (generated). */
	class MovementSensor : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "movementSensor";

		MovementSensor(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedMovements = MakeReadonlyProperty<&MovementSensor::SupportedMovements>([this]() { return _supportedMovements; });
		ReactiveLitepp::ReadonlyProperty<std::string> Movement = MakeReadonlyProperty<&MovementSensor::Movement>([this]() { return _movement; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedMovements;
		std::string _movement;
	};

} // namespace standard
} // namespace smartthings4cpp
