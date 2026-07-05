#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.audioGroupInfo capability (Samsung-proprietary; generated). */
	class AudioGroupInfo : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.audioGroupInfo";

		AudioGroupInfo(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> Role = MakeReadonlyProperty<&AudioGroupInfo::Role>([this]() { return _role; });
		ReactiveLitepp::ReadonlyProperty<std::string> Channel = MakeReadonlyProperty<&AudioGroupInfo::Channel>([this]() { return _channel; });
		ReactiveLitepp::ReadonlyProperty<std::string> Status = MakeReadonlyProperty<&AudioGroupInfo::Status>([this]() { return _status; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _role;
		std::string _channel;
		std::string _status;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
