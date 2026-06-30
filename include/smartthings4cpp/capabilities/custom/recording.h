#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.recording capability (Samsung-proprietary; generated). */
	class Recording : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.recording";

		Recording(int version, std::string componentId, std::string deviceId, Client* client);

		Result<void> recordStart();
		Result<void> recordStop();
		std::vector<std::string> commandNames() const override { return { "recordStart", "recordStop" }; }
	};

} // namespace custom
} // namespace smartthings4cpp
