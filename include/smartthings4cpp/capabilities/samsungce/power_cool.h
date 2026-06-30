#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.powerCool capability (Samsung-proprietary; generated). */
	class PowerCool : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.powerCool";

		PowerCool(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<bool> Activated{ [this]() { return _activated; } };

		Result<void> activate();
		Result<void> deactivate();
		std::vector<std::string> commandNames() const override { return { "activate", "deactivate" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		bool _activated = false;
	};

} // namespace samsungce
} // namespace smartthings4cpp
