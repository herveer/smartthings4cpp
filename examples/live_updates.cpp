// Silence MSVC's std::getenv deprecation warning in this demo.
#define _CRT_SECURE_NO_WARNINGS

/**
 * @file live_updates.cpp
 * @brief Automatic live updates example for smartthings4cpp
 *
 * SmartThings only delivers real push notifications (subscriptions/webhooks)
 * to an OAuth2-authorized app; a Personal Access Token cannot subscribe. This
 * example shows the PAT-friendly stand-in instead: by default, Client polls
 * every device it hands out in the background, and the existing reactive
 * property system only fires PropertyChanged for attributes that actually
 * changed - so "get notified when it changes" is exactly this:
 *   1. Construct a Client (polling starts automatically)
 *   2. Subscribe to a capability's PropertyChanged
 *   3. Go change something (open a door, flip a switch in the app) and watch
 *      the console print it, with the property already holding the new value
 *
 * No explicit poller to construct or start - see PollingMode in client.h if
 * you want to opt out and drive updates yourself via Client::pollNow().
 */

#include <smartthings4cpp/smartthings4cpp.h>

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace smartthings4cpp;

namespace {

	const std::string TOKEN_FILE = "smartthings_token.txt";
	constexpr std::chrono::seconds POLL_INTERVAL{ 7 };
	constexpr std::chrono::seconds WATCH_DURATION{ 10000 };

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

} // namespace

int main() {
	std::cout << "=== smartthings4cpp Live Updates Example (v" << Version::STRING << ") ===\n\n";

	std::string token = resolveToken();
	if (token.empty()) {
		std::cerr << "No access token provided. Aborting.\n";
		return 1;
	}

	Client client(token); // PollingMode::Automatic by default - already polling
	client.setPollingInterval(std::chrono::duration_cast<std::chrono::milliseconds>(POLL_INTERVAL));

	std::cout << "Validating token...\n";
	if (auto validation = client.validateAuthentication(); !validation.isSuccess()) {
		std::cerr << "Authentication failed: " << validation.error_message << "\n";
		return 1;
	}

	auto allDevices = client.getDevices();
	if (allDevices.empty()) {
		std::cout << "No device found to watch.\n";
		return 0;
	}

	std::cout << "\nWatching " << allDevices.size() << " device(s):\n";
	std::vector<ScopedSubscription> subscriptions;

	std::function<void(ObservableObject&, PropertyChangedArgs)> callback = [](ObservableObject& obj, PropertyChangedArgs args) {
		smartthings4cpp::Capability& capability = static_cast<smartthings4cpp::Capability&>(obj);
		std::string deviceLabel = "?";
		smartthings4cpp::Device* device = nullptr;
		if (auto* component = capability.component()) {
			if (device = component->device) {
				deviceLabel = device->Label.Get();
			}
		}

		std::cout << deviceLabel << " / " << capability.capabilityId() << ": " <<
			args.PropertyName() << " changed, new value: " << capability.statusJson().dump(2) << "\n";
		};

	for (const auto& device : allDevices) {
		std::cout << "  - " << device->Label.Get() << " (" << device->Id.Get() << ")\n";
		// One subscription per device: Device::PropertyChanged relays every
		// component's PropertyChanged, which in turn relays every one of its
		// capabilities' - so this alone hears about every attribute change
		// anywhere on the device, no manual loop over components/capabilities needed.
		device->PropertyChanged += callback;
	}

	std::cout << "\nPolling every " << POLL_INTERVAL.count() << "s for " << WATCH_DURATION.count()
		<< "s total (already running - no explicit start() needed). Go open/close\n"
		<< "a door or flip a switch in the SmartThings app now...\n\n";

	std::this_thread::sleep_for(WATCH_DURATION);

	// Stop before printing final state so nothing mutates mid-print.
	client.stopPolling();
	return 0;
}
