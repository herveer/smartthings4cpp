#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.washerAutoDetergent capability (Samsung-proprietary; generated). */
	class WasherAutoDetergent : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.washerAutoDetergent";

		WasherAutoDetergent(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> WasherAutoDetergentValue{ [this]() { return _washerAutoDetergent; } };

		Result<void> setWasherAutoDetergent(const std::string& washerAutoDetergent);
		std::vector<std::string> commandNames() const override { return { "setWasherAutoDetergent" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _washerAutoDetergent;
	};

} // namespace custom
} // namespace smartthings4cpp
