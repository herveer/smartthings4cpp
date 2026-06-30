#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.launchService capability (Samsung-proprietary; generated). */
	class LaunchService : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.launchService";

		LaunchService(int version, std::string componentId, std::string deviceId, Client* client);

		Result<void> launchTVPlus(const std::string& providerId, const std::string& serviceId);
		std::vector<std::string> commandNames() const override { return { "launchTVPlus" }; }
	};

} // namespace samsungvd
} // namespace smartthings4cpp
