#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.accessibility capability (Samsung-proprietary; generated). */
	class Accessibility : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.accessibility";

		Accessibility(int version, std::string componentId, std::string deviceId, Client* client);

		Result<void> setAccessibility(const std::string& menuName, const std::string& menuId, const std::string& option);
		std::vector<std::string> commandNames() const override { return { "setAccessibility" }; }
	};

} // namespace custom
} // namespace smartthings4cpp
