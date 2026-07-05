#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.washerBubbleSoak capability (Samsung-proprietary; generated). */
	class WasherBubbleSoak : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.washerBubbleSoak";

		WasherBubbleSoak(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> Status = MakeReadonlyProperty<&WasherBubbleSoak::Status>([this]() { return _status; });

		Result<void> off();
		Result<void> on();
		std::vector<std::string> commandNames() const override { return { "off", "on" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _status;
	};

} // namespace samsungce
} // namespace smartthings4cpp
