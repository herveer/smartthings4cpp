// Silence MSVC's std::getenv deprecation warning in this demo.
#define _CRT_SECURE_NO_WARNINGS

/**
 * @file device_control.cpp
 * @brief Read typed capability state and actively send a (self-restoring) command
 *
 * This example demonstrates the device-control iteration of smartthings4cpp:
 *   1. Authenticate with a Personal Access Token (PAT)
 *   2. Pick a target device (env SMARTTHINGS_DEVICE_ID, else the first device with a
 *      recognized controllable capability)
 *   3. Fetch its status and print typed attribute values for known capabilities
 *   4. Send a REAL command chosen to be self-reversing, and show before/after state
 *
 * The command is chosen to restore the original state automatically:
 *   - audioVolume: setVolume(...) then setVolume(original)
 *   - switch:      on()/off() flip, then flipped back
 *   - refresh:     refresh() (no state change)
 *
 * The PAT needs r:devices:* (read), x:devices:* (commands) and r:locations:* scopes.
 * Generate one at https://account.smartthings.com/tokens.
 */

#include <smartthings4cpp/smartthings4cpp.h>

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

using namespace smartthings4cpp;
using namespace smartthings4cpp::standard; // typed standard capabilities (Switch, AudioVolume, ...)

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

	// Print typed attribute values for the capabilities this iteration understands.
	void printState(Device& device) {
		for (const auto& component : device.getComponents()) {
			std::cout << "  [" << component.id << "]\n";
			if (auto* sw = component.get<Switch>())
				std::cout << "      switch:          " << sw->SwitchValue.Get() << "\n";
			if (auto* level = component.get<SwitchLevel>())
				std::cout << "      level:           " << level->Level.Get() << "%\n";
			if (auto* vol = component.get<AudioVolume>())
				std::cout << "      volume:          " << vol->Volume.Get() << "%\n";
			if (auto* m = component.get<AudioMute>())
				std::cout << "      mute:            " << m->Mute.Get() << "\n";
			if (auto* contact = component.get<ContactSensor>())
				std::cout << "      contact:         " << contact->Contact.Get() << "\n";
			if (auto* temp = component.get<TemperatureMeasurement>())
				std::cout << "      temperature:     " << temp->Temperature.Get() << "\n";
			if (auto* sp = component.get<ThermostatCoolingSetpoint>())
				std::cout << "      coolingSetpoint: " << sp->CoolingSetpoint.Get() << "\n";
		}
	}

	// Report a command result; returns true on success.
	bool report(const std::string& action, const Result<void>& result) {
		if (result.isSuccess()) {
			std::cout << "  -> " << action << ": OK\n";
			return true;
		}
		std::cout << "  -> " << action << ": FAILED (" << result.error_message << ")\n";
		return false;
	}

	// Send a real, self-reversing command on the device's "main" component.
	void demoCommand(Device& device) {
		std::cout << "\n--- Sending a self-restoring command ---\n";

		if (auto* vol = device.getCapability<AudioVolume>()) {
			int original = vol->Volume.Get();
			std::cout << "  audioVolume before: " << original << "%\n";
			for (int i = 0; i < 20; i = i + 3) {
				if (report("volumeUp()", vol->setVolume(i))) {
					device.refreshStatus();
					std::cout << "  audioVolume now:    " << vol->Volume.Get() << "%\n";
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
			}

			report("setVolume(restore)", vol->setVolume(original));
			device.refreshStatus();
			std::cout << "  audioVolume after:  " << vol->Volume.Get() << "%\n";
			return;
		}

		if (auto* sw = device.getCapability<Switch>()) {
			std::string original = sw->SwitchValue.Get();
			std::cout << "  switch before: " << original << "\n";
			auto flip = [&](const std::string& state) { return state == "on" ? sw->off() : sw->on(); };
			if (report("flip()", flip(original))) {
				device.refreshStatus();
				std::cout << "  switch now:    " << sw->SwitchValue.Get() << "\n";
				report("flip() back", flip(sw->SwitchValue.Get()));
				device.refreshStatus();
				std::cout << "  switch after:  " << sw->SwitchValue.Get() << "\n";
			}
			return;
		}

		if (auto* refresh = device.getCapability<Refresh>()) {
			report("refresh()", refresh->refresh());
			return;
		}

		std::cout << "  No controllable capability (audioVolume/switch/refresh) on 'main'.\n";
	}

} // namespace

int main() {
	std::cout << "=== smartthings4cpp Device Control Example (v" << Version::STRING << ") ===\n\n";

	std::string token = resolveToken();
	if (token.empty()) {
		std::cerr << "No access token provided. Aborting.\n";
		return 1;
	}

	Client client(token);
	std::cout << "Validating token...\n";
	if (auto validation = client.validateAuthentication(); !validation.isSuccess()) {
		std::cerr << "Authentication failed: " << validation.error_message << "\n";
		return 1;
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
			if (candidate->hasCapability<AudioVolume>()) {
				device = std::move(candidate);
				break;
			}
		}
	}

	if (!device) {
		std::cout << "No suitable device found to control.\n";
		return 0;
	}

	std::cout << "\nTarget device: " << device->Label.Get()
		<< " (" << device->Type.Get() << ")\n";
	std::cout << "  id: " << device->Id.Get() << "\n\n";

	std::cout << "--- Current state ---\n";
	if (!device->refreshStatus().isSuccess()) {
		std::cout << "  (could not fetch live status)\n";
	}
	printState(*device);

	demoCommand(*device);

	std::cout << "\nDone!\n";
	return 0;
}
