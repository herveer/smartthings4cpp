#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.tvsearch capability (Samsung-proprietary; generated). */
	class Tvsearch : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.tvsearch";

		Tvsearch(int version, std::string componentId, std::string deviceId, Client* client);

		Result<void> search(const std::string& query, const std::string& url);
		std::vector<std::string> commandNames() const override { return { "search" }; }
	};

} // namespace custom
} // namespace smartthings4cpp
