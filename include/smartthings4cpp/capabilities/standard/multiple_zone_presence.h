#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c multipleZonePresence capability (generated). */
	class MultipleZonePresence : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "multipleZonePresence";

		MultipleZonePresence(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ZoneState{ [this]() { return _zoneState; } };

		Result<void> deleteZone(const std::string& id);
		Result<void> createZone(const std::string& name, const std::string& id);
		Result<void> updateZoneName(const std::string& id, const std::string& name);
		std::vector<std::string> commandNames() const override { return { "deleteZone", "createZone", "updateZoneName" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _zoneState;
	};

} // namespace standard
} // namespace smartthings4cpp
