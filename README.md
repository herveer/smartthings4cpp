# smartthings4cpp

A modern, lightweight C++ library providing a developer-friendly interface to the
Samsung **SmartThings** public API.

## Overview

**smartthings4cpp** is a cross-platform C++ library designed to make working with
the SmartThings cloud simple and intuitive. 

> **Project status:** 🚧 Under active development. This first iteration covers
> **authentication and device discovery**. Device control (commands, live status)
> is the next iteration — see [ROADMAP.md](ROADMAP.md).

## Features (iteration 1)

- 🔐 **Token authentication** — connect with a SmartThings Personal Access Token (PAT)
- ✅ **Token validation** — verify a token against the cloud before using it
- 🔍 **Device discovery** — enumerate every device in your account (paginated)
- 🧩 **Capability discovery** — inspect each device's components and capabilities
- 🏠 **Locations & rooms** — resolve devices to their location and room
- 🔄 **Reactive objects** — `Device` is a `ReactiveLitepp::ObservableObject` exposing
  its metadata through observable properties
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
2. Create a token with at least the **`r:devices:*`** and **`r:locations:*`** scopes.
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

    for (const auto& device : client.getDevices()) {
        std::cout << device->Label.Get() << " (" << device->Type.Get() << ")\n";
        for (const auto& capability : device->getCapabilityIds()) {
            std::cout << "  - " << capability << "\n";
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
│   ├── client.h                # Cloud client: auth + discovery
│   ├── device.h                # Device (ObservableObject) + capabilities
│   ├── types.h                 # Result, ErrorCode, Capability, Component, ...
│   ├── exceptions.h            # Exception types
│   ├── http_client.h           # HTTP client wrapper (cpr)
│   └── json_utils.h            # JSON utilities
├── src/                        # Implementation
├── examples/                   # device_listing, authentication
└── tests/                      # Catch2 unit tests (offline)
```

## License

MIT — see [LICENSE](LICENSE).

---

**Note**: This library is not affiliated with or endorsed by Samsung or
SmartThings. All trademarks are property of their respective owners.
