#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.welcomeMessage capability (Samsung-proprietary; generated). */
	class WelcomeMessage : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.welcomeMessage";

		WelcomeMessage(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> WelcomeMessageValue = MakeReadonlyProperty<&WelcomeMessage::WelcomeMessageValue>([this]() { return _welcomeMessageValue; });

		Result<void> deleteWelcomeMessage();
		Result<void> setWelcomeMessage(const nlohmann::json& welcomeMessage);
		std::vector<std::string> commandNames() const override { return { "deleteWelcomeMessage", "setWelcomeMessage" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _welcomeMessageValue;
	};

} // namespace samsungce
} // namespace smartthings4cpp
