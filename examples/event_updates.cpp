// Silence MSVC's std::getenv deprecation warning in this demo.
#define _CRT_SECURE_NO_WARNINGS

/**
 * @file event_updates.cpp
 * @brief Real push updates over OAuth webhook events - the autonomous flow
 *
 * An OAuth-mode Client handles the whole story by itself: it embeds a local
 * HTTP server for the OAuth redirect and the SmartThings webhook, persists the
 * token pair across runs (Windows Credential Manager / file storage), and
 * subscribes/unsubscribes device events automatically as Device objects come
 * and go. The one thing it cannot do is open a browser - that's the only
 * manual step below.
 *
 * One-time setup for your OAuth-In App (created with `smartthings apps:create`):
 *   1. Register the redirect URI  http://localhost:9753/oauth/callback
 *   2. Run a tunnel so SmartThings can reach the webhook over public HTTPS:
 *        ngrok http 9753
 *      and set the app's Target URL to  https://<your-tunnel>/webhook
 *
 * Then run this example:
 *   - First run: it prints the authorize URL - open it, consent, done. The
 *     token is persisted, so later runs authenticate silently.
 *   - Toggle a device in the SmartThings app and watch PropertyChanged fire
 *     here, pushed by SmartThings - no polling anywhere.
 *
 * NOTE: the webhook payload signature is not yet cryptographically verified;
 * keep the endpoint behind your own tunnel.
 */

#include <smartthings4cpp/smartthings4cpp.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace smartthings4cpp;

namespace {

	constexpr int LISTEN_PORT = 9057;
	constexpr std::chrono::minutes AUTH_TIMEOUT{ 5 };
	constexpr std::chrono::seconds WATCH_DURATION{ 300 };

	std::string trim(const std::string& s) {
		const auto first = s.find_first_not_of(" \t\r\n");
		if (first == std::string::npos) {
			return "";
		}
		const auto last = s.find_last_not_of(" \t\r\n");
		return s.substr(first, last - first + 1);
	}

	std::string resolve(const char* envVar, const std::string& prompt) {
		if (const char* env = std::getenv(envVar)) {
			std::string value = trim(env);
			if (!value.empty()) {
				return value;
			}
		}
		std::cout << prompt;
		std::string value;
		std::getline(std::cin, value);
		return trim(value);
	}

} // namespace

int main() {
	std::cout << "=== smartthings4cpp Event Updates Example (v" << Version::STRING << ") ===\n\n";

	oauth2::OAuth2Config config;
	config.clientId = resolve("SMARTTHINGS_CLIENT_ID", "OAuth clientId: ");
	config.clientSecret = resolve("SMARTTHINGS_CLIENT_SECRET", "OAuth clientSecret: ");
	if (config.clientId.empty() || config.clientSecret.empty()) {
		std::cerr << "clientId/clientSecret are required (smartthings apps:create). Aborting.\n";
		return 1;
	}

	// Everything embedded: local server on port 9753 (OAuth redirect + webhook),
	// token persistence under ./st4cpp-data (secrets go to the OS keychain on
	// Windows). No polling - SmartThings pushes events to the webhook.
	Client client(config, LISTEN_PORT, "/oauth/callback", "/", "https://pantry-seventh-fidelity.ngrok-free.dev", "./st4cpp-data");

	// The single manual step of the whole flow.
	client.openBrowserRequested += [](std::string url) {
		std::cout << "\nAuthorization needed - open this URL in your browser:\n\n"
			<< "  " << url << "\n\n"
			<< "(the redirect lands on the embedded server; nothing to paste back)\n";
		};
	client.authenticationCompleted += [](Result<void> outcome) {
		if (outcome.isSuccess()) {
			std::cout << "Authenticated. Token persisted - next run will be silent.\n";
		}
		else {
			std::cout << "Authentication failed: " << outcome.error_message << "\n";
		}
		};

	auto auth = client.authenticate(); // silent when a stored token still works
	if (!auth.isSuccess()) {
		std::cerr << "Cannot authenticate: " << auth.error_message << "\n";
		return 1;
	}
	if (auth.hasValue() && !*auth.value) {
		std::cout << "Waiting for the browser round-trip...\n";
		if (auto waited = client.waitForAuthentication(AUTH_TIMEOUT); !waited.isSuccess()) {
			std::cerr << "Authentication did not complete: " << waited.error_message << "\n";
			return 1;
		}
	}

	// Devices are subscribed to automatically as they're created - and
	// unsubscribed when the last shared_ptr for each one goes away.
	auto devices = client.getDevices();
	if (devices.empty()) {
		std::cout << "No device found.\n";
		return 0;
	}

	std::cout << "\nWatching " << devices.size() << " device(s) - subscriptions created automatically:\n";
	std::vector<ScopedSubscription> subscriptions;
	for (const auto& device : devices) {
		std::cout << "  - " << device->Label.Get() << " (" << device->Id.Get() << ")\n";
		subscriptions.push_back(device->PropertyChanged.SubscribeScoped(
			[](ObservableObject& obj, PropertyChangedArgs args) {
				static std::mutex mutex;
				const auto lock = std::scoped_lock(mutex);
				auto& capability = static_cast<Capability&>(obj);
				std::string label = "?";
				if (auto* component = capability.component()) {
					if (auto* owner = component->device) {
						label = owner->Label.Get();
					}
				}

				std::any newValue = obj.GetProperty(args.PropertyName());

				// Convert any to a string for display, if possible
				std::string newValueStr = "?";
				if (newValue.has_value()) {
					if(newValue.type() == typeid(std::string)) {
						newValueStr = std::any_cast<std::string>(newValue);
					}
					else if(newValue.type() == typeid(int)) {
						newValueStr = std::to_string(std::any_cast<int>(newValue));
					}
					else if(newValue.type() == typeid(double)) {
						newValueStr = std::to_string(std::any_cast<double>(newValue));
					}
					else if(newValue.type() == typeid(bool)) {
						newValueStr = std::any_cast<bool>(newValue) ? "true" : "false";
					}
					else {
						newValueStr = "<unprintable type: " + std::string(newValue.type().name()) + ">";
					}
				}


				std::cout << "  [push] " << label << " / " << capability.component()->id << " / " << capability.capabilityId()
					<< ": " << args.PropertyName() << " changed, new value = " << newValueStr << std::endl;
			}));
	}

	std::cout << "\nListening for " << WATCH_DURATION.count() << "s. Make sure your tunnel is up\n"
		<< "(e.g. `ngrok http " << LISTEN_PORT << "`, Target URL https://<tunnel>/webhook),\n"
		<< "then toggle a device in the SmartThings app and watch it arrive here...\n\n";

	std::this_thread::sleep_for(WATCH_DURATION);

	std::cout << "Done.\n";
	return 0;
}
