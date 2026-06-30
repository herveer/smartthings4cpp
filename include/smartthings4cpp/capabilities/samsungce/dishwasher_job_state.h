#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.dishwasherJobState capability (Samsung-proprietary; generated). */
	class DishwasherJobState : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.dishwasherJobState";

		DishwasherJobState(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ScheduledJobs{ [this]() { return _scheduledJobs; } };
		ReactiveLitepp::ReadonlyProperty<std::string> DishwasherJobStateValue{ [this]() { return _dishwasherJobState; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _scheduledJobs;
		std::string _dishwasherJobState;
	};

} // namespace samsungce
} // namespace smartthings4cpp
