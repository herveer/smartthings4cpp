#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.softwareVersion capability (Samsung-proprietary; generated). */
	class SoftwareVersion : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.softwareVersion";

		SoftwareVersion(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Versions{ [this]() { return _versions; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> PlatformVersion{ [this]() { return _platformVersion; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _versions;
		nlohmann::json _platformVersion;
	};

} // namespace samsungce
} // namespace smartthings4cpp
