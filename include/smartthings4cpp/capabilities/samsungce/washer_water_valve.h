#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.washerWaterValve capability (Samsung-proprietary; generated). */
	class WasherWaterValve : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.washerWaterValve";

		WasherWaterValve(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> WaterValve{ [this]() { return _waterValve; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedWaterValve{ [this]() { return _supportedWaterValve; } };

		Result<void> setWaterValve(const std::string& waterValve);
		std::vector<std::string> commandNames() const override { return { "setWaterValve" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _waterValve;
		std::vector<std::string> _supportedWaterValve;
	};

} // namespace samsungce
} // namespace smartthings4cpp
