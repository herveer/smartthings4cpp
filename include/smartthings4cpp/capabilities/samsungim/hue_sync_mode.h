#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungim {

	/** @brief The @c samsungim.hueSyncMode capability (Samsung-proprietary; generated). */
	class HueSyncMode : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungim.hueSyncMode";

		HueSyncMode(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> Mode = MakeReadonlyProperty<&HueSyncMode::Mode>([this]() { return _mode; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _mode;
	};

} // namespace samsungim
} // namespace smartthings4cpp
