#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.soundDetection capability (Samsung-proprietary; generated). */
	class SoundDetection : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.soundDetection";

		SoundDetection(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> SoundDetected{ [this]() { return _soundDetected; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _soundDetected;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
