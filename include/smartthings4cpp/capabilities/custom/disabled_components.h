#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.disabledComponents capability (Samsung-proprietary; generated). */
	class DisabledComponents : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.disabledComponents";

		DisabledComponents(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> DisabledComponentsValue{ [this]() { return _disabledComponentsValue; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _disabledComponentsValue;
	};

} // namespace custom
} // namespace smartthings4cpp
