#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.washerSpinLevel capability (Samsung-proprietary; generated). */
	class WasherSpinLevel : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.washerSpinLevel";

		WasherSpinLevel(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> WasherSpinLevelValue{ [this]() { return _washerSpinLevelValue; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedWasherSpinLevel{ [this]() { return _supportedWasherSpinLevel; } };

		Result<void> setWasherSpinLevel(const std::string& spinLevel);
		std::vector<std::string> commandNames() const override { return { "setWasherSpinLevel" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _washerSpinLevelValue;
		std::vector<std::string> _supportedWasherSpinLevel;
	};

} // namespace custom
} // namespace smartthings4cpp
