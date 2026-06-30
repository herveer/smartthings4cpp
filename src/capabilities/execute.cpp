#include "smartthings4cpp/capabilities/execute.h"

#include <utility>

namespace smartthings4cpp {

	Execute::Execute(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("execute", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Execute::parseStatus(const nlohmann::json&) {
		auto data = attributeValue("data");
		if (!data.is_null()) {
			SetPropertyValueAndNotify<&Execute::Data>(_data, data);
		}
	}

	Result<void> Execute::execute(const std::string& command, const nlohmann::json& args) {
		nlohmann::json arguments = nlohmann::json::array({ command });
		// args is optional; only include it when it carries something.
		if (!args.is_null() && !(args.is_object() && args.empty())) {
			arguments.push_back(args);
		}
		return executeCommand("execute", arguments);
	}

} // namespace smartthings4cpp
