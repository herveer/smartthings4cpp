#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c presenceSensor capability (generated). */
	class PresenceSensor : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "presenceSensor";

		PresenceSensor(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> Presence{ [this]() { return _presence; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _presence;
	};

} // namespace standard
} // namespace smartthings4cpp
