#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c switch capability (generated). */
	class Switch : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "switch";

		Switch(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> SwitchValue = MakeReadonlyProperty<&Switch::SwitchValue>([this]() { return _switchValue; });

		Result<void> off();
		Result<void> on();
		std::vector<std::string> commandNames() const override { return { "off", "on" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _switchValue;
	};

} // namespace standard
} // namespace smartthings4cpp
