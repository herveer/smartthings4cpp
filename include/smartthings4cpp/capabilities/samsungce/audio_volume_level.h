#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.audioVolumeLevel capability (Samsung-proprietary; generated). */
	class AudioVolumeLevel : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.audioVolumeLevel";

		AudioVolumeLevel(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> VolumeLevel = MakeReadonlyProperty<&AudioVolumeLevel::VolumeLevel>([this]() { return _volumeLevel; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> VolumeLevelRange = MakeReadonlyProperty<&AudioVolumeLevel::VolumeLevelRange>([this]() { return _volumeLevelRange; });

		Result<void> volumeDown();
		Result<void> volumeUp();
		Result<void> setVolumeLevel(int volumeLevel);
		std::vector<std::string> commandNames() const override { return { "volumeDown", "volumeUp", "setVolumeLevel" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _volumeLevel = 0;
		nlohmann::json _volumeLevelRange;
	};

} // namespace samsungce
} // namespace smartthings4cpp
