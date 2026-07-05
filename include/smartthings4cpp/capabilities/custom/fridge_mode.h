#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.fridgeMode capability (Samsung-proprietary; generated). */
	class FridgeMode : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.fridgeMode";

		FridgeMode(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> FridgeModeValue = MakeReadonlyProperty<&FridgeMode::FridgeModeValue>([this]() { return _fridgeModeValue; });
		ReactiveLitepp::ReadonlyProperty<std::string> FridgeModeValue_ = MakeReadonlyProperty<&FridgeMode::FridgeModeValue_>([this]() { return _fridgeModeValue_; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedFridgeOptions = MakeReadonlyProperty<&FridgeMode::SupportedFridgeOptions>([this]() { return _supportedFridgeOptions; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedFullFridgeModes = MakeReadonlyProperty<&FridgeMode::SupportedFullFridgeModes>([this]() { return _supportedFullFridgeModes; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedFridgeModes = MakeReadonlyProperty<&FridgeMode::SupportedFridgeModes>([this]() { return _supportedFridgeModes; });

		Result<void> setFridgeMode(const std::string& fridgeMode);
		std::vector<std::string> commandNames() const override { return { "setFridgeMode" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _fridgeModeValue;
		std::string _fridgeModeValue_;
		std::vector<std::string> _supportedFridgeOptions;
		std::vector<std::string> _supportedFullFridgeModes;
		std::vector<std::string> _supportedFridgeModes;
	};

} // namespace custom
} // namespace smartthings4cpp
