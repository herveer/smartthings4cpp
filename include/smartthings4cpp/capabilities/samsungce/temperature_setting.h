#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.temperatureSetting capability (Samsung-proprietary; generated). */
	class TemperatureSetting : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.temperatureSetting";

		TemperatureSetting(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedDesiredTemperatures = MakeReadonlyProperty<&TemperatureSetting::SupportedDesiredTemperatures>([this]() { return _supportedDesiredTemperatures; });
		ReactiveLitepp::ReadonlyProperty<double> DesiredTemperature = MakeReadonlyProperty<&TemperatureSetting::DesiredTemperature>([this]() { return _desiredTemperature; });

		Result<void> setDesiredTemperature(double desiredTemperature);
		std::vector<std::string> commandNames() const override { return { "setDesiredTemperature" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _supportedDesiredTemperatures;
		double _desiredTemperature = 0.0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
