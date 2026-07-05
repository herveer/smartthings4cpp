#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.washerAutoSoftener capability (Samsung-proprietary; generated). */
	class WasherAutoSoftener : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.washerAutoSoftener";

		WasherAutoSoftener(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> WasherAutoSoftenerValue{ [this]() { return _washerAutoSoftenerValue; } };

		Result<void> setWasherAutoSoftener(const std::string& washerAutoSoftener);
		std::vector<std::string> commandNames() const override { return { "setWasherAutoSoftener" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _washerAutoSoftenerValue;
	};

} // namespace custom
} // namespace smartthings4cpp
