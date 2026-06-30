#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.dongleSoftwareInstallation capability (Samsung-proprietary; generated). */
	class DongleSoftwareInstallation : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.dongleSoftwareInstallation";

		DongleSoftwareInstallation(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> Status{ [this]() { return _status; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _status;
	};

} // namespace samsungce
} // namespace smartthings4cpp
