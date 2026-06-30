#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.driverState capability (Samsung-proprietary; generated). */
	class DriverState : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.driverState";

		DriverState(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> DriverStateValue{ [this]() { return _driverState; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _driverState;
	};

} // namespace samsungce
} // namespace smartthings4cpp
