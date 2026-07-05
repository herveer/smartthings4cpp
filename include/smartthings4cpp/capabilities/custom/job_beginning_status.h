#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.jobBeginningStatus capability (Samsung-proprietary; generated). */
	class JobBeginningStatus : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.jobBeginningStatus";

		JobBeginningStatus(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> JobBeginningStatusValue{ [this]() { return _jobBeginningStatusValue; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _jobBeginningStatusValue;
	};

} // namespace custom
} // namespace smartthings4cpp
