#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace sec {

	/** @brief The @c sec.diagnosticsInformation capability (Samsung-proprietary; generated). */
	class DiagnosticsInformation : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "sec.diagnosticsInformation";

		DiagnosticsInformation(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> LogType{ [this]() { return _logType; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Endpoint{ [this]() { return _endpoint; } };
		ReactiveLitepp::ReadonlyProperty<std::string> MinVersion{ [this]() { return _minVersion; } };
		ReactiveLitepp::ReadonlyProperty<std::string> SigninPermission{ [this]() { return _signinPermission; } };
		ReactiveLitepp::ReadonlyProperty<std::string> SetupId{ [this]() { return _setupId; } };
		ReactiveLitepp::ReadonlyProperty<std::string> ProtocolType{ [this]() { return _protocolType; } };
		ReactiveLitepp::ReadonlyProperty<std::string> TsId{ [this]() { return _tsId; } };
		ReactiveLitepp::ReadonlyProperty<std::string> MnId{ [this]() { return _mnId; } };
		ReactiveLitepp::ReadonlyProperty<std::string> DumpType{ [this]() { return _dumpType; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _logType;
		std::string _endpoint;
		std::string _minVersion;
		std::string _signinPermission;
		std::string _setupId;
		std::string _protocolType;
		std::string _tsId;
		std::string _mnId;
		std::string _dumpType;
	};

} // namespace sec
} // namespace smartthings4cpp
