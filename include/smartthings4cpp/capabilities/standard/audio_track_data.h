#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c audioTrackData capability (generated). */
	class AudioTrackData : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "audioTrackData";

		AudioTrackData(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> TotalTime = MakeReadonlyProperty<&AudioTrackData::TotalTime>([this]() { return _totalTime; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> AudioTrackDataValue = MakeReadonlyProperty<&AudioTrackData::AudioTrackDataValue>([this]() { return _audioTrackDataValue; });
		ReactiveLitepp::ReadonlyProperty<int> ElapsedTime = MakeReadonlyProperty<&AudioTrackData::ElapsedTime>([this]() { return _elapsedTime; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _totalTime = 0;
		nlohmann::json _audioTrackDataValue;
		int _elapsedTime = 0;
	};

} // namespace standard
} // namespace smartthings4cpp
