#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c audioVolume capability (generated). */
	class AudioVolume : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "audioVolume";

		AudioVolume(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> Volume = MakeReadonlyProperty<&AudioVolume::Volume>([this]() { return _volume; });

		Result<void> volumeDown();
		Result<void> volumeUp();
		Result<void> setVolume(int volume);
		std::vector<std::string> commandNames() const override { return { "volumeDown", "volumeUp", "setVolume" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _volume = 0;
	};

} // namespace standard
} // namespace smartthings4cpp
