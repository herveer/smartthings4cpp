#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.washerSoilLevel capability (Samsung-proprietary; generated). */
	class WasherSoilLevel : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.washerSoilLevel";

		WasherSoilLevel(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedWasherSoilLevel{ [this]() { return _supportedWasherSoilLevel; } };
		ReactiveLitepp::ReadonlyProperty<std::string> WasherSoilLevelValue{ [this]() { return _washerSoilLevel; } };

		Result<void> setWasherSoilLevel(const std::string& soilLevel);
		std::vector<std::string> commandNames() const override { return { "setWasherSoilLevel" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedWasherSoilLevel;
		std::string _washerSoilLevel;
	};

} // namespace custom
} // namespace smartthings4cpp
