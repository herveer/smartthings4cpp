#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.firmwareVersion capability (Samsung-proprietary; generated). */
	class FirmwareVersion : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.firmwareVersion";

		FirmwareVersion(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> FirmwareVersionValue = MakeReadonlyProperty<&FirmwareVersion::FirmwareVersionValue>([this]() { return _firmwareVersionValue; });

		Result<void> setFirmwareVersion(const std::string& version);
		std::vector<std::string> commandNames() const override { return { "setFirmwareVersion" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _firmwareVersionValue;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
