#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.quickControl capability (Samsung-proprietary; generated). */
	class QuickControl : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.quickControl";

		QuickControl(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> VersionAttr{ [this]() { return _version; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _version;
	};

} // namespace samsungce
} // namespace smartthings4cpp
