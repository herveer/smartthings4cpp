#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c demandResponseLoadControl capability (generated). */
	class DemandResponseLoadControl : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "demandResponseLoadControl";

		DemandResponseLoadControl(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> DrlcStatus = MakeReadonlyProperty<&DemandResponseLoadControl::DrlcStatus>([this]() { return _drlcStatus; });

		Result<void> overrideDrlcAction(bool value);
		Result<void> requestDrlcAction(int drlcType, int drlcLevel, const std::string& start, int duration, int reportingPeriod);
		std::vector<std::string> commandNames() const override { return { "overrideDrlcAction", "requestDrlcAction" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _drlcStatus;
	};

} // namespace standard
} // namespace smartthings4cpp
