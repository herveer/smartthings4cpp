#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.washerCyclePreset capability (Samsung-proprietary; generated). */
	class WasherCyclePreset : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.washerCyclePreset";

		WasherCyclePreset(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<double> MaxNumberOfPresets = MakeReadonlyProperty<&WasherCyclePreset::MaxNumberOfPresets>([this]() { return _maxNumberOfPresets; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Presets = MakeReadonlyProperty<&WasherCyclePreset::Presets>([this]() { return _presets; });

		Result<void> setWasherCyclePreset(const nlohmann::json& presets);
		Result<void> delete_(const nlohmann::json& presetIds);
		std::vector<std::string> commandNames() const override { return { "setWasherCyclePreset", "delete" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		double _maxNumberOfPresets = 0.0;
		nlohmann::json _presets;
	};

} // namespace samsungce
} // namespace smartthings4cpp
