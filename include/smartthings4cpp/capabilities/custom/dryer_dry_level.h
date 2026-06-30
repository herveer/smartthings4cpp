#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.dryerDryLevel capability (Samsung-proprietary; generated). */
	class DryerDryLevel : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.dryerDryLevel";

		DryerDryLevel(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> DryerDryLevelValue{ [this]() { return _dryerDryLevel; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedDryerDryLevel{ [this]() { return _supportedDryerDryLevel; } };

		Result<void> setDryerDryLevel(const std::string& dryLevel);
		std::vector<std::string> commandNames() const override { return { "setDryerDryLevel" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _dryerDryLevel;
		std::vector<std::string> _supportedDryerDryLevel;
	};

} // namespace custom
} // namespace smartthings4cpp
