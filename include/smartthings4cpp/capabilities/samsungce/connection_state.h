#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.connectionState capability (Samsung-proprietary; generated). */
	class ConnectionState : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.connectionState";

		ConnectionState(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> ConnectionStateValue{ [this]() { return _connectionState; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _connectionState;
	};

} // namespace samsungce
} // namespace smartthings4cpp
