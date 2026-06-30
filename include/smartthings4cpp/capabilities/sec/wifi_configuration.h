#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace sec {

	/** @brief The @c sec.wifiConfiguration capability (Samsung-proprietary; generated). */
	class WifiConfiguration : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "sec.wifiConfiguration";

		WifiConfiguration(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<bool> AutoReconnection{ [this]() { return _autoReconnection; } };
		ReactiveLitepp::ReadonlyProperty<std::string> MinVersion{ [this]() { return _minVersion; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedWiFiFreq{ [this]() { return _supportedWiFiFreq; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedAuthType{ [this]() { return _supportedAuthType; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> ProtocolType{ [this]() { return _protocolType; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		bool _autoReconnection = false;
		std::string _minVersion;
		std::vector<std::string> _supportedWiFiFreq;
		std::vector<std::string> _supportedAuthType;
		std::vector<std::string> _protocolType;
	};

} // namespace sec
} // namespace smartthings4cpp
