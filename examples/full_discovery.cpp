// Silence MSVC's std::getenv deprecation warning in this demo.
#define _CRT_SECURE_NO_WARNINGS

/**
 * @file full_discovery.cpp
 * @brief Dump every device, every capability, and every attribute value + command
 *
 * Connects with a Personal Access Token, then for every device fetches its live
 * status and prints, per capability, all attribute values and all command names:
 *
 *   <device_name>:
 *       <capability_name>:
 *           attributes:
 *               <attr_name>: <value or list of values>
 *               ...
 *           commands:
 *               <command_name>
 *               ...
 *       ...
 *   ...
 *
 * Attribute values come from GET /v1/devices/{id}/status (so only attributes that
 * currently have a value are shown). Command names come from each capability class's
 * commandNames() reflection helper. Capabilities without a typed class (none of the
 * standard/Samsung ones, but possible for other vendors) still list their attributes;
 * their commands appear empty.
 *
 * The PAT needs at least r:devices:* scope. Generate one at
 * https://account.smartthings.com/tokens.
 */

#include <smartthings4cpp/smartthings4cpp.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace smartthings4cpp;

namespace {

const std::string TOKEN_FILE = "smartthings_token.txt";
const std::string IND1 = "    ";       // device-level indent step
const std::string IND2 = IND1 + IND1;  // capability
const std::string IND3 = IND2 + IND1;  // "attributes:" / "commands:"
const std::string IND4 = IND3 + IND1;  // entries

std::string trim(const std::string& s) {
	const auto first = s.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) {
		return "";
	}
	const auto last = s.find_last_not_of(" \t\r\n");
	return s.substr(first, last - first + 1);
}

std::string resolveToken() {
	if (const char* env = std::getenv("SMARTTHINGS_TOKEN")) {
		std::string token = trim(env);
		if (!token.empty()) {
			return token;
		}
	}
	std::ifstream file(TOKEN_FILE);
	if (file.is_open()) {
		std::string token;
		std::getline(file, token);
		token = trim(token);
		if (!token.empty()) {
			return token;
		}
	}
	std::cout << "Enter your SmartThings Personal Access Token: ";
	std::string token;
	std::getline(std::cin, token);
	return trim(token);
}

// Render any attribute value (scalar, array, or object) as a single line.
std::string renderValue(const nlohmann::json& v) {
	if (v.is_null()) {
		return "(none)";
	}
	if (v.is_string()) {
		return v.get<std::string>();
	}
	if (v.is_boolean()) {
		return v.get<bool>() ? "true" : "false";
	}
	if (v.is_number()) {
		return v.dump();
	}
	if (v.is_array()) {
		std::string out = "[";
		bool first = true;
		for (const auto& e : v) {
			out += (first ? "" : ", ") + renderValue(e);
			first = false;
		}
		return out + "]";
	}
	return v.dump(); // object -> compact JSON
}

void printCapability(const Capability& capability, const std::string& componentId) {
	// Capability label: prefix with the component when it isn't the main one.
	std::string name = capability.capabilityId();
	if (componentId != "main") {
		name = componentId + "/" + name;
	}
	std::cout << IND2 << name << ":\n";

	std::cout << IND3 << "attributes:\n";
	const auto& status = capability.statusJson();
	if (status.is_object() && !status.empty()) {
		for (const auto& [attr, node] : status.items()) {
			nlohmann::json value = (node.is_object() && node.contains("value"))
				? node["value"] : nlohmann::json(nullptr);
			std::string line = IND4 + attr + ": " + renderValue(value);
			if (node.is_object() && node.contains("unit") && node["unit"].is_string()) {
				const std::string unit = node["unit"].get<std::string>();
				if (!unit.empty()) {
					line += " " + unit;
				}
			}
			std::cout << line << "\n";
		}
	} else {
		std::cout << IND4 << "(no attribute values)\n";
	}

	std::cout << IND3 << "commands:\n";
	const auto commands = capability.commandNames();
	if (!commands.empty()) {
		for (const auto& command : commands) {
			std::cout << IND4 << command << "\n";
		}
	} else {
		std::cout << IND4 << "(none)\n";
	}
}

} // namespace

int main() {
	std::cout << "=== smartthings4cpp Full Discovery (v" << Version::STRING << ") ===\n\n";

	std::string token = resolveToken();
	if (token.empty()) {
		std::cerr << "No access token provided. Aborting.\n";
		return 1;
	}

	Client client(token);
	if (auto validation = client.validateAuthentication(); !validation.isSuccess()) {
		std::cerr << "Authentication failed: " << validation.error_message << "\n";
		return 1;
	}

	auto devices = client.getDevices();
	if (devices.empty()) {
		std::cout << "No devices found.\n";
		return 0;
	}

	for (const auto& device : devices) {
		// Pull live attribute values for this device's capabilities.
		device->refreshStatus();

		const std::string label = device->Label.Get();
		std::cout << (label.empty() ? device->Id.Get() : label) << ":\n";

		for (const auto& component : device->getComponents()) {
			for (const auto& capability : component.capabilities) {
				if (capability) {
					printCapability(*capability, component.id);
				}
			}
		}
		std::cout << "\n";
	}

	return 0;
}
