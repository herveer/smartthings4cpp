#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.sabbathMode capability (Samsung-proprietary; generated). */
	class SabbathMode : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.sabbathMode";

		SabbathMode(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedActions{ [this]() { return _supportedActions; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Status{ [this]() { return _status; } };

		Result<void> off();
		Result<void> on();
		std::vector<std::string> commandNames() const override { return { "off", "on" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedActions;
		std::string _status;
	};

} // namespace samsungce
} // namespace smartthings4cpp
