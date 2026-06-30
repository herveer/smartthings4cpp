#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.fridgeWelcomeLighting capability (Samsung-proprietary; generated). */
	class FridgeWelcomeLighting : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.fridgeWelcomeLighting";

		FridgeWelcomeLighting(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> DetectionProximity{ [this]() { return _detectionProximity; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedDetectionProximities{ [this]() { return _supportedDetectionProximities; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Status{ [this]() { return _status; } };

		Result<void> setDetectionProximity(const std::string& detectionProximity);
		Result<void> off();
		Result<void> on();
		std::vector<std::string> commandNames() const override { return { "setDetectionProximity", "off", "on" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _detectionProximity;
		std::vector<std::string> _supportedDetectionProximities;
		std::string _status;
	};

} // namespace samsungce
} // namespace smartthings4cpp
