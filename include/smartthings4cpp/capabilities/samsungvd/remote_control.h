#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.remoteControl capability (Samsung-proprietary; generated). */
	class RemoteControl : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.remoteControl";

		RemoteControl(int version, std::string componentId, std::string deviceId, Client* client);

		Result<void> send(const std::string& keyValue, const std::string& keyState);
		std::vector<std::string> commandNames() const override { return { "send" }; }
	};

} // namespace samsungvd
} // namespace smartthings4cpp
