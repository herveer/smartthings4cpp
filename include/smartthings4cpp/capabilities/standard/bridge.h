#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c bridge capability (generated). */
	class Bridge : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "bridge";

		Bridge(int version, std::string componentId, std::string deviceId, Client* client);
	};

} // namespace standard
} // namespace smartthings4cpp
