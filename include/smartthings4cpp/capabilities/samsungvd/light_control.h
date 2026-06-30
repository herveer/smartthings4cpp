#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.lightControl capability (Samsung-proprietary; generated). */
	class LightControl : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.lightControl";

		LightControl(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedModeMap{ [this]() { return _supportedModeMap; } };
		ReactiveLitepp::ReadonlyProperty<std::string> RequestId{ [this]() { return _requestId; } };
		ReactiveLitepp::ReadonlyProperty<std::string> SelectedMode{ [this]() { return _selectedMode; } };
		ReactiveLitepp::ReadonlyProperty<bool> StreamControl{ [this]() { return _streamControl; } };
		ReactiveLitepp::ReadonlyProperty<std::string> SelectedAppId{ [this]() { return _selectedAppId; } };
		ReactiveLitepp::ReadonlyProperty<std::string> ErrorCode{ [this]() { return _errorCode; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedModes{ [this]() { return _supportedModes; } };

		Result<void> setLightControlMode(const std::string& selectedMode);
		std::vector<std::string> commandNames() const override { return { "setLightControlMode" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _supportedModeMap;
		std::string _requestId;
		std::string _selectedMode;
		bool _streamControl = false;
		std::string _selectedAppId;
		std::string _errorCode;
		nlohmann::json _supportedModes;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
