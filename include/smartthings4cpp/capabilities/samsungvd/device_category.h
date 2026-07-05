#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.deviceCategory capability (Samsung-proprietary; generated). */
	class DeviceCategory : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.deviceCategory";

		DeviceCategory(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> CategoryAttr{ [this]() { return _categoryAttr; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _categoryAttr;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
