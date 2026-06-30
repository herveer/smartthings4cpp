#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.driverVersion capability (Samsung-proprietary; generated). */
	class DriverVersion : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.driverVersion";

		DriverVersion(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<double> VersionNumber{ [this]() { return _versionNumber; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		double _versionNumber = 0.0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
