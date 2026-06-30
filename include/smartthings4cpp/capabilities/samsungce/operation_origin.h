#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.operationOrigin capability (Samsung-proprietary; generated). */
	class OperationOrigin : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.operationOrigin";

		OperationOrigin(int version, std::string componentId, std::string deviceId, Client* client);

		Result<void> setOperationOrigin(const std::string& origin, const nlohmann::json& data);
		std::vector<std::string> commandNames() const override { return { "setOperationOrigin" }; }
	};

} // namespace samsungce
} // namespace smartthings4cpp
