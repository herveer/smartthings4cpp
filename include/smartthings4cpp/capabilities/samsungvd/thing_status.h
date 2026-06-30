#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.thingStatus capability (Samsung-proprietary; generated). */
	class ThingStatus : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.thingStatus";

		ThingStatus(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> UpdatedTime{ [this]() { return _updatedTime; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Status{ [this]() { return _status; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _updatedTime = 0;
		std::string _status;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
