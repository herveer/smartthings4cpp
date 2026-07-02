# smartthings4cpp

A modern, lightweight C++ library providing a developer-friendly interface to the
Samsung **SmartThings** public API.

## Overview

**smartthings4cpp** is a cross-platform C++ library designed to make working with
the SmartThings cloud simple and intuitive. 

> **Project status:** 🚧 Under active development. Authentication, device discovery,
> and typed **device control** for a first batch of capabilities are implemented.
> See [ROADMAP.md](ROADMAP.md) for the remaining capabilities and upcoming work.

## Features

- 🔐 **Token authentication** — connect with a SmartThings Personal Access Token (PAT)
  or a self-registered **OAuth 2.0** app (authorize URL, code exchange, refresh, and
  transparent re-authentication on `Client` via `setTokenRefreshCallback`)
- ✅ **Token validation** — verify a token against the cloud before using it
- 🔍 **Device discovery** — enumerate every device in your account (paginated)
- 🧩 **Capability classes** — each device capability is a typed object exposing
  getters (attributes) and setters/commands; `Capability` is the polymorphic base
- 🎚️ **Device control** — read live status (`GET …/status`) and send commands
  (`POST …/commands`) through **126 typed capability classes**: 19 standard ones
  (`switch`, `switchLevel`, `audioVolume`, `audioMute`, `contactSensor`,
  `temperatureMeasurement`, `thermostatCoolingSetpoint`, `mediaPlayback`, `tvChannel`,
  `refrigeration`, `washerOperatingState`, …) plus **107 Samsung-proprietary ones** in
  per-prefix sub-namespaces (`samsungce::*`, `samsungvd::*`, `custom::*`, `sec::*`,
  `hca::*`, `samsungim::*`). Any unrecognised id falls back to a generic capability.
- 🏠 **Locations & rooms** — resolve devices to their location and room
- 🔄 **Reactive objects** — `Device` and every `Capability` are
  `ReactiveLitepp::ObservableObject`s exposing observable properties
- 📡 **Automatic live updates (polling)** — `Client` polls every device it hands
  out in the background by default (opt out with `PollingMode::Manual`, or drive
  updates yourself with `pollNow()`); `PropertyChanged` fires only for attributes
  that actually changed, and fetching the same device id more than once still
  polls it only once per cycle. Works under a PAT — SmartThings only pushes real
  subscription/webhook events to an OAuth2-authorized app
- 📦 **vcpkg + CMake** — easy dependency management and integration
- 🔧 **Cross-platform** — Windows, Linux, and macOS

## Requirements

- **C++ Compiler**: C++17 or later (GCC 8+, Clang 7+, MSVC 2019+)
- **CMake**: 3.16 or later
- **vcpkg**: for dependency management

## Dependencies

All dependencies are managed via vcpkg:

- **cpr** — HTTP client (built on libcurl)
- **nlohmann-json** — JSON parsing and serialization
- **reactivelitepp** — reactive properties / observable objects
- **Catch2** — unit testing framework

`reactivelitepp` is resolved through the `herve-er/vcpkg-registery` registry pinned
in `vcpkg-configuration.json` (a backup `overlay-ports/reactivelitepp` port is also
provided).

## Getting a Personal Access Token (PAT)

1. Go to <https://account.smartthings.com/tokens>.
2. Create a token with at least the **`r:devices:*`**, **`x:devices:*`** (to send
   commands) and **`r:locations:*`** scopes.
3. Copy the token — you will not be able to see it again.

> ⚠️ PATs created after **30 December 2024 expire 24 hours after creation**. If a
> saved token stops working, generate a new one. For long-lived production
> integrations, use the OAuth 2.0 authorization-code flow (see below).

## Using OAuth2

> ⚠️ **Currently blocked for new apps:** Samsung is mid-migration from classic
> SmartApps to a new "API Integration App" model, and as of 2026-07 there is no
> way to create a new OAuth2 app at all. The flow below is implemented and
> ready, but you can't provision an app to use it with yet — see
> [Live updates (polling)](#live-updates-polling) for the working alternative
> in the meantime.

For longer-lived integrations, register your own SmartThings **OAuth-In App**
instead of using a PAT — the same self-service model, just with a refreshable
token instead of a token that expires after 24 hours with no way to renew it:

1. Install the [SmartThings CLI](https://github.com/SmartThingsCommunity/smartthings-cli)
   and run `smartthings apps:create`, selecting **OAuth-In App**. Give it the
   **`r:devices:*`**, **`x:devices:*`** and **`r:locations:*`** scopes and a
   redirect URI (e.g. `http://localhost:9753/callback` — see the caveat below).
2. Save the `clientId`/`clientSecret` it gives you.
3. Use `smartthings4cpp::oauth2::OAuth2Authenticator`:

```cpp
#include <smartthings4cpp/smartthings4cpp.h>

using namespace smartthings4cpp;
using namespace smartthings4cpp::oauth2;

OAuth2Config config;
config.clientId = "...";
config.clientSecret = "...";
OAuth2Authenticator authenticator(config);

// 1. Build the authorize URL and present it however you like (open a
//    browser, print it, load it in a webview, ...) - the library has no
//    opinion here.
std::string url = authenticator.buildAuthorizeUrl("http://localhost:9753/callback");

// 2. However you captured what came back from the redirect (a local
//    listener, the user pasting the URL back, a mobile deep link, ...),
//    hand it to completeAuthorization(). It accepts the full redirect URL
//    or a bare code, and validates the CSRF state for you.
Result<OAuth2Token> result = authenticator.completeAuthorization(redirect_result);

// 3. Use the token, and optionally wire transparent refresh so Client
//    self-heals past the access token's ~24h expiry instead of failing.
Client client;
client.setAccessToken(result.value->accessToken);
client.setTokenRefreshCallback([&]() -> Result<std::string> {
    auto refreshed = authenticator.refresh(result.value->refreshToken);
    if (!refreshed.isSuccess()) return Result<std::string>(refreshed.error, refreshed.error_message);
    return Result<std::string>(refreshed.value->accessToken);
});
```

Run the full working version with `examples/oauth2_authentication` (env vars
`SMARTTHINGS_CLIENT_ID`/`SMARTTHINGS_CLIENT_SECRET`, or it will prompt and save
them locally). It persists the token to `oauth2_token.json` and refreshes it
automatically on later runs.

> ⚠️ SmartThings' docs say redirect URIs must be HTTPS, but a plain
> `http://localhost:PORT/...` redirect URI has worked for self-registered
> personal apps in practice — if yours is rejected at registration time,
> register any HTTPS URL you control instead (even one that 404s) and copy
> the `code` out of the browser's address bar the same way.

## Live updates (polling)

SmartThings only pushes real subscription/webhook events to an OAuth2-authorized
app. With just a PAT (or while OAuth2 app creation is blocked, see above),
`Client` is the working stand-in: **by default it automatically polls every
device it hands out**, in the background, no separate poller to construct or
start. Because the reactive property system only fires `PropertyChanged` when
a value actually changes, you get "notified only on a real change" for free —
and because `Client` deduplicates by device id internally, calling
`getDevice()`/`getDevices()` more than once for the same device still polls it
only once per cycle, updating every `Device` object you got back for it:

```cpp
#include <smartthings4cpp/smartthings4cpp.h>

using namespace smartthings4cpp;

Client client("your-personal-access-token"); // PollingMode::Automatic by default
auto devices = client.getDevices();

auto sensor = devices.front()->getCapability<ContactSensor>();
auto sub = sensor->PropertyChanged.SubscribeScoped(
    [](ObservableObject&, PropertyChangedArgs args) {
        std::cout << "changed: " << args.PropertyName() << "\n";
    });

// Already polling - nothing else to start. Adjust the interval if you want:
client.setPollingInterval(std::chrono::seconds(10));
// ... later, e.g. before shutdown ...
client.stopPolling();
```

Construct with `Client client(token, PollingMode::Manual);` to opt out of
automatic polling entirely - `Client::pollNow()` is then available to drive a
single poll cycle yourself (your own timer/thread) whenever you want one.
Run `examples/live_updates` for a full working version.

## Building

```bash
git clone https://github.com/herveer/smartthings4cpp.git
cd smartthings4cpp

cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

Build options: `BUILD_TESTS` (ON), `BUILD_EXAMPLES` (ON), `BUILD_SHARED_LIBS` (OFF).

### Running the tests

The unit tests are offline (they parse fixture JSON and need no token or network):

```bash
ctest --test-dir build --output-on-failure
```

## Running the demo

The `device_listing` example connects to your account and prints every device and
its capabilities. Provide your PAT via the `SMARTTHINGS_TOKEN` environment variable,
a `smartthings_token.txt` file, or the interactive prompt:

```bash
# Windows (PowerShell)
$env:SMARTTHINGS_TOKEN = "<your-pat>"
./build/examples/device_listing

# Linux / macOS
export SMARTTHINGS_TOKEN="<your-pat>"
./build/examples/device_listing
```

Example output:

```
Location: Home (3 device(s))
  - Living Room Lamp
      id:           11111111-2222-3333-4444-555555555555
      type:         ZIGBEE
      manufacturer: SmartThings
      room:         Living Room
      capabilities:
        [main] switch, switchLevel, refresh
```

The `device_control` example fetches a device's live status, prints typed attribute
values, and sends a real **self-restoring** command (e.g. nudges and restores the
volume, or toggles a switch twice). Target a specific device with
`SMARTTHINGS_DEVICE_ID`, otherwise it picks the first controllable device:

```bash
$env:SMARTTHINGS_TOKEN = "<your-pat>"     # needs x:devices:* scope for commands
./build/examples/device_control
```

The `device_control` example shows how to control a single device. The
`full_discovery` example instead **dumps every device, every capability, every
attribute value and command** (read-only, `r:devices:*` is enough):

```bash
$env:SMARTTHINGS_TOKEN = "<your-pat>"
./build/examples/full_discovery
```

```
Lave-linge:
    switch:
        attributes:
            switch: off
        commands:
            on
            off
    washerOperatingState:
        attributes:
            completionTime: 2026-06-30T22:24:49Z
            machineState: stop
            supportedMachineStates: [stop, run, pause]
            washerJobState: none
        commands:
            setMachineState
    samsungce.washerOperatingState:
        attributes:
            operatingState: ready
            progress: 1 %
            scheduledJobs: [{"jobName":"wash","timeInMin":80}, ...]
            supportedOperatingStates: [ready, running, paused]
        commands:
            resume
            ...
    ...
```

(Capabilities on non-`main` components are shown as `<component>/<capability>`.)

The `live_updates` example watches every `contactSensor`/`switch` device (or one
picked via `SMARTTHINGS_DEVICE_ID`), relying on `Client`'s automatic background
polling to print each attribute change as it's detected — open/close a door or
flip a switch in the SmartThings app while it's running to see it:

```bash
$env:SMARTTHINGS_TOKEN = "<your-pat>"
./build/examples/live_updates
```

## Usage

```cpp
#include <smartthings4cpp/smartthings4cpp.h>
#include <iostream>

int main() {
    using namespace smartthings4cpp;

    Client client("your-personal-access-token");

    if (!client.validateAuthentication().isSuccess()) {
        std::cerr << "Invalid token\n";
        return 1;
    }

    auto devices = client.getDevices();
    for (const auto& device : devices) {
        // getCapability()/getComponent() auto-fetch the device's status the first
        // time they're called, so attribute getters below are never stale/empty.
        // Call device->refreshStatus() explicitly whenever you want a later re-fetch.

        // Typed control: get a capability by its concrete type.
        if (auto* sw = device->getCapability<Switch>()) {
            std::cout << device->Label.Get() << " is " << sw->State.Get() << "\n";
            sw->on();                                   // POST a command
        }
        if (auto* vol = device->getCapability<AudioVolume>()) {
            vol->setVolume(20);                         // setter == command
        }
    }
    return 0;
}
```

## Project Structure

```
smartthings4cpp/
├── CMakeLists.txt              # Root CMake configuration
├── vcpkg.json                  # Dependency manifest
├── vcpkg-configuration.json    # Registries (incl. reactivelitepp)
├── overlay-ports/              # Backup vcpkg port for reactivelitepp
├── include/smartthings4cpp/    # Public headers
│   ├── smartthings4cpp.h       # Umbrella header
│   ├── client.h                # Cloud client: auth + discovery + status/commands
│   ├── device.h                # Device (ObservableObject) + components/capabilities
│   ├── component.h             # Component: owns its capabilities
│   ├── capability.h            # Capability base class + factory
│   ├── capabilities.h          # Umbrella for all typed capabilities
│   ├── capabilities/           # Standard caps (Switch, SwitchLevel, ...) +
│   │                           #   samsungce/ samsungvd/ custom/ sec/ hca/ samsungim/
│   │                           #   (Samsung-proprietary caps, generated)
│   ├── types.h                 # Result, ErrorCode, Category, Location, Room
│   ├── exceptions.h            # Exception types
│   ├── http_client.h           # HTTP client wrapper (cpr)
│   ├── json_utils.h            # JSON utilities
│   ├── base64.h                # Minimal base64 (OAuth2 Basic Auth header)
│   └── oauth2/                 # OAuth2Config, OAuth2Token, OAuth2Authenticator
├── src/                        # Implementation (incl. src/capabilities/, src/oauth2/)
├── examples/                   # device_listing, authentication, oauth2_authentication,
│                               #   device_control, full_discovery, live_updates
├── tools/                      # Generator for the proprietary capabilities
└── tests/                      # Catch2 unit tests (offline)
```

## License

MIT — see [LICENSE](LICENSE).

---

**Note**: This library is not affiliated with or endorsed by Samsung or
SmartThings. All trademarks are property of their respective owners.
