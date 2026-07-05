#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c remoteControlStatus capability (generated). */
	class RemoteControlStatus : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "remoteControlStatus";

		RemoteControlStatus(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> RemoteControlEnabled{ [this]() { return _remoteControlEnabled; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _remoteControlEnabled;
	};

} // namespace standard
} // namespace smartthings4cpp
