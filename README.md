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
> integrations, use the OAuth 2.0 authorization-code flow (planned — see the roadmap).

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
        device->refreshStatus();                       // fetch live attribute values

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
│   └── json_utils.h            # JSON utilities
├── src/                        # Implementation (incl. src/capabilities/)
├── examples/                   # device_listing, authentication, device_control, full_discovery
├── tools/                      # Generator for the proprietary capabilities
└── tests/                      # Catch2 unit tests (offline)
```

## License

MIT — see [LICENSE](LICENSE).

---

**Note**: This library is not affiliated with or endorsed by Samsung or
SmartThings. All trademarks are property of their respective owners.
