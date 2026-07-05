#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.disabledCapabilities capability (Samsung-proprietary; generated). */
	class DisabledCapabilities : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.disabledCapabilities";

		DisabledCapabilities(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> DisabledCapabilitiesValue{ [this]() { return _disabledCapabilitiesValue; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _disabledCapabilitiesValue;
	};

} // namespace custom
} // namespace smartthings4cpp
