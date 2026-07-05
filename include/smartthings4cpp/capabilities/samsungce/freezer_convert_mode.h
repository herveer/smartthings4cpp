#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.freezerConvertMode capability (Samsung-proprietary; generated). */
	class FreezerConvertMode : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.freezerConvertMode";

		FreezerConvertMode(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedFreezerConvertModes = MakeReadonlyProperty<&FreezerConvertMode::SupportedFreezerConvertModes>([this]() { return _supportedFreezerConvertModes; });
		ReactiveLitepp::ReadonlyProperty<std::string> FreezerConvertModeValue = MakeReadonlyProperty<&FreezerConvertMode::FreezerConvertModeValue>([this]() { return _freezerConvertModeValue; });

		Result<void> setFreezerConvertMode(const std::string& freezerConvertMode);
		std::vector<std::string> commandNames() const override { return { "setFreezerConvertMode" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedFreezerConvertModes;
		std::string _freezerConvertModeValue;
	};

} // namespace samsungce
} // namespace smartthings4cpp
