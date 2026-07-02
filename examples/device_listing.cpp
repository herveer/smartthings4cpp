// Silence MSVC's std::getenv deprecation warning in this demo.
#define _CRT_SECURE_NO_WARNINGS

/**
 * @file device_listing.cpp
 * @brief Connect to a SmartThings account and list every device and its capabilities
 *
 * This example demonstrates the first iteration of smartthings4cpp:
 *   1. Authenticate with a Personal Access Token (PAT)
 *   2. Validate the token against the SmartThings cloud
 *   3. Discover locations and rooms (to give devices a friendly home)
 *   4. Enumerate every device and print its components and capabilities
 *
 * The token is resolved, in order, from:
 *   - the SMARTTHINGS_TOKEN environment variable
 *   - a local "smartthings_token.txt" file (one line: the token)
 *   - an interactive prompt (the entered token is then saved to that file)
 *
 * Generate a PAT at https://account.smartthings.com/tokens with at least the
 * "r:devices:*" and "r:locations:*" scopes.
 *
 * NOTE: PATs created after 30 December 2024 expire 24 hours after creation, so a
 * saved token may need to be regenerated between runs.
 */

#include <smartthings4cpp/smartthings4cpp.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace smartthings4cpp;

namespace {

	const std::string TOKEN_FILE = "smartthings_token.txt";

	std::string trim(const std::string& s) {
		const auto first = s.find_first_not_of(" \t\r\n");
		if (first == std::string::npos) {
			return "";
		}
		const auto last = s.find_last_not_of(" \t\r\n");
		return s.substr(first, last - first + 1);
	}

	std::string loadTokenFromFile() {
		std::ifstream file(TOKEN_FILE);
		if (!file.is_open()) {
			return "";
		}
		std::string token;
		std::getline(file, token);
		return trim(token);
	}

	bool saveTokenToFile(const std::string& token) {
		std::ofstream file(TOKEN_FILE);
		if (!file.is_open()) {
			return false;
		}
		file << token << std::endl;
		return true;
	}

	// Resolve the access token from env var, file, then interactive prompt.
	std::string resolveToken() {
		if (const char* env = std::getenv("SMARTTHINGS_TOKEN")) {
			std::string token = trim(env);
			if (!token.empty()) {
				std::cout << "Using token from SMARTTHINGS_TOKEN environment variable.\n";
				return token;
			}
		}

		std::string token = loadTokenFromFile();
		if (!token.empty()) {
			std::cout << "Using token from " << TOKEN_FILE << ".\n";
			return token;
		}

		std::cout << "Enter your SmartThings Personal Access Token: ";
		std::getline(std::cin, token);
		token = trim(token);
		if (!token.empty() && saveTokenToFile(token)) {
			std::cout << "Token saved to " << TOKEN_FILE << ".\n";
		}
		return token;
	}

	void printDevice(const Device& device,
		const std::map<std::string, std::string>& roomNames) {
		const std::string label = device.Label.Get();
		std::cout << "  - " << (label.empty() ? "(unnamed device)" : label) << "\n";
		std::cout << "      id:           " << device.Id.Get() << "\n";
		std::cout << "      type:         " << device.Type.Get() << "\n";

		const std::string manufacturer = device.ManufacturerName.Get();
		if (!manufacturer.empty()) {
			std::cout << "      manufacturer: " << manufacturer << "\n";
		}

		const std::string roomId = device.RoomId.Get();
		if (!roomId.empty()) {
			auto it = roomNames.find(roomId);
			std::cout << "      room:         "
				<< (it != roomNames.end() ? it->second : roomId) << "\n";
		}

		std::cout << "      capabilities:\n";
		for (const auto& component : device.getComponents()) {
			std::cout << "        [" << component.id << "] ";
			if (component.capabilities.empty()) {
				std::cout << "(none)";
			}
			else {
				bool first = true;
				for (const auto& capability : component.capabilities) {
					std::cout << (first ? "" : ", ") << capability->capabilityId();
					first = false;
				}
			}
			std::cout << "\n";
		}
	}

} // namespace

int main() {
	std::cout << "=== smartthings4cpp Device Listing Example (v"
		<< Version::STRING << ") ===\n\n";

	// Step 1: resolve and configure the access token.
	std::string token = resolveToken();
	if (token.empty()) {
		std::cerr << "No access token provided. Aborting.\n";
		return 1;
	}

	Client client(token);

	// Step 2: validate the token.
	std::cout << "\nValidating token with the SmartThings API...\n";
	auto validation = client.validateAuthentication();
	if (!validation.isSuccess()) {
		std::cerr << "Authentication failed: " << validation.error_message << "\n";
		std::cerr << "Generate a fresh PAT at https://account.smartthings.com/tokens "
			"with the r:devices:* and r:locations:* scopes.\n";
		return 1;
	}
	std::cout << "Token is valid.\n";

	// Step 3: discover locations and rooms for friendlier output.
	std::map<std::string, std::string> locationNames;
	std::map<std::string, std::string> roomNames;
	for (const auto& location : client.getLocations()) {
		locationNames[location.locationId] = location.name;
		for (const auto& room : client.getRooms(location.locationId)) {
			roomNames[room.roomId] = room.name;
		}
	}

	// Step 4: enumerate devices, grouped by location.
	std::cout << "\nDiscovering devices...\n";
	auto devices = client.getDevices();
	if (devices.empty()) {
		std::cout << "No devices found on this account.\n";
		return 0;
	}

	std::map<std::string, std::vector<const Device*>> byLocation;
	for (const auto& device : devices) {
		byLocation[device->LocationId.Get()].push_back(device.get());
	}

	std::cout << "Found " << devices.size() << " device(s).\n";
	for (const auto& [locationId, locationDevices] : byLocation) {
		std::string locationLabel = "(no location)";
		if (!locationId.empty()) {
			auto it = locationNames.find(locationId);
			locationLabel = (it != locationNames.end() ? it->second : locationId);
		}

		std::cout << "\nLocation: " << locationLabel
			<< " (" << locationDevices.size() << " device(s))\n";
		for (const auto* device : locationDevices) {
			printDevice(*device, roomNames);
		}
	}

	// Select a target device.
	std::shared_ptr<Device> device;
	if (const char* envId = std::getenv("SMARTTHINGS_DEVICE_ID")) {
		std::string id = trim(envId);
		if (!id.empty()) {
			device = client.getDevice(id);
		}
	}
	if (!device) {
		// First device with a capability we can safely demonstrate.
		for (auto& candidate : client.getDevices()) {
			if (candidate->hasCapability<samsungce::TemperatureSetting>() && candidate->hasCapability<TemperatureMeasurement>()) {
				device = std::move(candidate);
				break;
			}
		}
	}

	if (!device) {
		std::cout << "No suitable device found to control.\n";
		return 0;
	}

	std::cout << "\nSelected device for control demonstration:\n";
	printDevice(*device, roomNames);
	device->refreshStatus();
	std::cout << "\nTemperatureSetting capability state: "
		<< device->getCapability<samsungce::TemperatureSetting>("cooler")->DesiredTemperature.Get()
		<< std::endl << device->getCapability<TemperatureMeasurement>("cooler")->Temperature.Get() << std::endl;

	std::cout << "\nDone!\n";
	return 0;
}
