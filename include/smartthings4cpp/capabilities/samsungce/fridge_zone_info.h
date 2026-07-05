#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.fridgeZoneInfo capability (Samsung-proprietary; generated). */
	class FridgeZoneInfo : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.fridgeZoneInfo";

		FridgeZoneInfo(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> Name = MakeReadonlyProperty<&FridgeZoneInfo::Name>([this]() { return _name; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _name;
	};

} // namespace samsungce
} // namespace smartthings4cpp
