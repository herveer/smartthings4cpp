#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.washerWashingTime capability (Samsung-proprietary; generated). */
	class WasherWashingTime : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.washerWashingTime";

		WasherWashingTime(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedWashingTimes{ [this]() { return _supportedWashingTimes; } };
		ReactiveLitepp::ReadonlyProperty<std::string> WashingTime{ [this]() { return _washingTime; } };

		Result<void> setWashingTime(const std::string& washingTime);
		std::vector<std::string> commandNames() const override { return { "setWashingTime" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedWashingTimes;
		std::string _washingTime;
	};

} // namespace samsungce
} // namespace smartthings4cpp
