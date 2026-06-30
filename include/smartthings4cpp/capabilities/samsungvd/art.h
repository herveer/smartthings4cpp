#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.art capability (Samsung-proprietary; generated). */
	class Art : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.art";

		Art(int version, std::string componentId, std::string deviceId, Client* client);

		Result<void> setArtOn();
		std::vector<std::string> commandNames() const override { return { "setArtOn" }; }
	};

} // namespace samsungvd
} // namespace smartthings4cpp
