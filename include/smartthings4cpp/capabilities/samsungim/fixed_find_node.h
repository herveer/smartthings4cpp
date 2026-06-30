#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungim {

	/** @brief The @c samsungim.fixedFindNode capability (Samsung-proprietary; generated). */
	class FixedFindNode : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungim.fixedFindNode";

		FixedFindNode(int version, std::string componentId, std::string deviceId, Client* client);

		Result<void> refresh();
		std::vector<std::string> commandNames() const override { return { "refresh" }; }
	};

} // namespace samsungim
} // namespace smartthings4cpp
