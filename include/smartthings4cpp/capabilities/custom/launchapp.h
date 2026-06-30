#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.launchapp capability (Samsung-proprietary; generated). */
	class Launchapp : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.launchapp";

		Launchapp(int version, std::string componentId, std::string deviceId, Client* client);

		Result<void> launchAppCommand(const std::string& appId, const std::string& appName);
		std::vector<std::string> commandNames() const override { return { "launchApp" }; }
	};

} // namespace custom
} // namespace smartthings4cpp
