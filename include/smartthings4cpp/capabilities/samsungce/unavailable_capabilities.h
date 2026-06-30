#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.unavailableCapabilities capability (Samsung-proprietary; generated). */
	class UnavailableCapabilities : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.unavailableCapabilities";

		UnavailableCapabilities(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> UnavailableCommands{ [this]() { return _unavailableCommands; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _unavailableCommands;
	};

} // namespace samsungce
} // namespace smartthings4cpp
