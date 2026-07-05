#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c healthCheck capability (generated). */
	class HealthCheck : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "healthCheck";

		HealthCheck(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> CheckInterval{ [this]() { return _checkInterval; } };
		ReactiveLitepp::ReadonlyProperty<std::string> HealthStatus{ [this]() { return _healthStatus; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> DeviceWatchEnroll{ [this]() { return _deviceWatchEnroll; } };
		ReactiveLitepp::ReadonlyProperty<std::string> DeviceWatchDeviceStatus{ [this]() { return _deviceWatchDeviceStatus; } };

		Result<void> ping();
		std::vector<std::string> commandNames() const override { return { "ping" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _checkInterval = 0;
		std::string _healthStatus;
		nlohmann::json _deviceWatchEnroll;
		std::string _deviceWatchDeviceStatus;
	};

} // namespace standard
} // namespace smartthings4cpp
