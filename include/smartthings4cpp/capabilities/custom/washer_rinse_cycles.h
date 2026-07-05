#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.washerRinseCycles capability (Samsung-proprietary; generated). */
	class WasherRinseCycles : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.washerRinseCycles";

		WasherRinseCycles(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedWasherRinseCycles{ [this]() { return _supportedWasherRinseCycles; } };
		ReactiveLitepp::ReadonlyProperty<std::string> WasherRinseCyclesValue{ [this]() { return _washerRinseCyclesValue; } };

		Result<void> setWasherRinseCycles(const std::string& cycle);
		std::vector<std::string> commandNames() const override { return { "setWasherRinseCycles" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedWasherRinseCycles;
		std::string _washerRinseCyclesValue;
	};

} // namespace custom
} // namespace smartthings4cpp
