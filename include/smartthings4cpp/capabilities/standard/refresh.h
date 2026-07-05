#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c refresh capability (generated). */
	class Refresh : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "refresh";

		Refresh(int version, std::string componentId, std::string deviceId, Client* client);

		Result<void> refresh();
		std::vector<std::string> commandNames() const override { return { "refresh" }; }
	};

} // namespace standard
} // namespace smartthings4cpp
