#include "smartthings4cpp/capability.h"
#include "smartthings4cpp/capabilities.h"
#include "smartthings4cpp/client.h"

#include <utility>

namespace smartthings4cpp {

	Capability::Capability(std::string capabilityId, int version, std::string componentId,
		std::string deviceId, Client* client)
		: _capabilityId(std::move(capabilityId)),
		_version(version),
		_componentId(std::move(componentId)),
		_deviceId(std::move(deviceId)),
		_client(client) {
	}

	void Capability::parseStatus(const nlohmann::json&) {
		// Default: nothing to parse (UnknownCapability and command-only capabilities).
	}

	void Capability::updateStatus(const nlohmann::json& capStatus) {
		if (capStatus.is_object()) {
			_status = capStatus;
		}
		parseStatus(_status);
	}

	nlohmann::json Capability::attributeValue(const std::string& attr) const {
		if (_status.is_object() && _status.contains(attr) && _status[attr].is_object()
			&& _status[attr].contains("value")) {
			return _status[attr]["value"];
		}
		return nlohmann::json(nullptr);
	}

	std::string Capability::attributeString(const std::string& attr, const std::string& def) const {
		auto value = attributeValue(attr);
		return value.is_string() ? value.get<std::string>() : def;
	}

	double Capability::attributeNumber(const std::string& attr, double def) const {
		auto value = attributeValue(attr);
		return value.is_number() ? value.get<double>() : def;
	}

	bool Capability::attributeBool(const std::string& attr, bool def) const {
		auto value = attributeValue(attr);
		return value.is_boolean() ? value.get<bool>() : def;
	}

	std::string Capability::attributeUnit(const std::string& attr, const std::string& def) const {
		if (_status.is_object() && _status.contains(attr) && _status[attr].is_object()
			&& _status[attr].contains("unit") && _status[attr]["unit"].is_string()) {
			return _status[attr]["unit"].get<std::string>();
		}
		return def;
	}

	std::vector<std::string> Capability::attributeStringArray(const std::string& attr) const {
		std::vector<std::string> result;
		auto value = attributeValue(attr);
		if (value.is_array()) {
			for (const auto& item : value) {
				if (item.is_string()) {
					result.push_back(item.get<std::string>());
				}
			}
		}
		return result;
	}

	void Capability::setLocalAttribute(const std::string& attr, const nlohmann::json& value) {
		if (!_status.is_object()) {
			_status = nlohmann::json::object();
		}
		_status[attr]["value"] = value;
		parseStatus(_status);
	}

	Result<void> Capability::refreshStatus() {
		if (!_client) {
			return Result<void>(ErrorCode::InvalidParameter, "No client associated with capability");
		}

		auto status = _client->getCapabilityStatus(_deviceId, _componentId, _capabilityId);
		if (status.is_null()) {
			return Result<void>(ErrorCode::NetworkError, "Failed to fetch capability status");
		}

		updateStatus(status);
		return Result<void>();
	}

	Result<void> Capability::executeCommand(const std::string& command, const nlohmann::json& args) {
		if (!_client) {
			return Result<void>(ErrorCode::InvalidParameter, "No client associated with capability");
		}

		auto body = buildCommandsBody(_componentId, _capabilityId, command, args);
		return _client->executeCommands(_deviceId, body);
	}

	nlohmann::json buildCommandsBody(const std::string& component,
		const std::string& capability,
		const std::string& command,
		const nlohmann::json& args) {
		nlohmann::json arguments = args.is_array() ? args : nlohmann::json::array();

		nlohmann::json entry = {
			{ "component", component },
			{ "capability", capability },
			{ "command", command },
			{ "arguments", arguments }
		};

		return nlohmann::json{ { "commands", nlohmann::json::array({ entry }) } };
	}

	std::unique_ptr<Capability> createCapability(const std::string& id, int version,
		const std::string& componentId, const std::string& deviceId, Client* client) {
		// Every typed capability is generated (see tools/generate_capabilities.ps1):
		// standard ids first, then the Samsung-proprietary ids, else UnknownCapability.
		if (auto standard = createStandardCapability(id, version, componentId, deviceId, client)) {
			return standard;
		}

		if (auto proprietary = createProprietaryCapability(id, version, componentId, deviceId, client)) {
			return proprietary;
		}

		return std::make_unique<UnknownCapability>(id, version, componentId, deviceId, client);
	}

} // namespace smartthings4cpp
