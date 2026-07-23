[📚 Documentation](README.md) › **Getting started**

# Getting started

This guide takes you from a clean machine to a running program that talks to
your SmartThings account.

- [Requirements](#requirements)
- [Building the library](#building-the-library)
- [Running the tests](#running-the-tests)
- [Using smartthings4cpp in your own project](#using-smartthings4cpp-in-your-own-project)
- [Your first program](#your-first-program)
- [Build options](#build-options)

---

## Requirements

| Tool | Minimum version |
|---|---|
| C++ compiler | C++17 — GCC 8+, Clang 7+, or MSVC 2019+ |
| [CMake](https://cmake.org/) | 3.16 |
| [vcpkg](https://vcpkg.io/) | any recent checkout |

All library dependencies are resolved by vcpkg from the manifest
([`vcpkg.json`](../vcpkg.json)):

| Package | Used for |
|---|---|
| **cpr** | Outbound HTTP (built on libcurl) |
| **cpp-httplib** | The embedded OAuth/webhook server — an implementation detail, never exposed in public headers |
| **nlohmann-json** | JSON parsing and serialization |
| **reactivelitepp** | Reactive properties / observable objects |
| **Catch2** | Unit tests only |

> [!NOTE]
> `reactivelitepp` is not in the default vcpkg registry. It is resolved through
> the `herveer/vcpkg-registery` registry pinned in
> [`vcpkg-configuration.json`](../vcpkg-configuration.json).

---

## Building the library

```bash
git clone https://github.com/herveer/smartthings4cpp.git
cd smartthings4cpp

cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

On the first configure, vcpkg downloads and builds the dependencies — expect a
few minutes.

<details>
<summary><b>Windows: Visual Studio notes</b></summary>

The repository works out of the box with Visual Studio's *Open Folder* CMake
support (`CMakeSettings.json` is included). From a *Developer PowerShell*:

```powershell
cmake -B build -S . -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake"
cmake --build build
```

If `cmake`/`ninja` are not on your `PATH`, launch the *Developer* prompt (or run
`VsDevCmd.bat`) first — Visual Studio ships its own copies.

</details>

---

## Running the tests

The unit tests are **fully offline**: they parse fixture JSON and exercise the
protocol logic without a token or any network access.

```bash
ctest --test-dir build --output-on-failure
```

---

## Using smartthings4cpp in your own project

### Option A — vcpkg + `find_package` (recommended)

Install the library, then consume the exported CMake package:

```bash
cmake --build build --target install
```

```cmake
find_package(smartthings4cpp CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE smartthings4cpp::smartthings4cpp)
```

### Option B — `add_subdirectory`

Vendor the repository (submodule or copy) and add it directly. Your project
must be configured with the vcpkg toolchain so the dependencies resolve:

```cmake
set(BUILD_TESTS OFF)
set(BUILD_EXAMPLES OFF)
add_subdirectory(external/smartthings4cpp)
target_link_libraries(my_app PRIVATE smartthings4cpp)
```

Either way, a single include pulls in the whole public API:

```cpp
#include <smartthings4cpp/smartthings4cpp.h>
```

---

## Your first program

You need a token first — the quickest (but short-lived) way is a
[Personal Access Token](authentication.md#personal-access-tokens-discouraged);
the durable way is an [OAuth-In App](authentication.md#oauth-recommended).
With a token in hand:

```cpp
#include <smartthings4cpp/smartthings4cpp.h>
#include <iostream>

int main() {
    using namespace smartthings4cpp;

    Client client("your-token");

    if (!client.validateAuthentication()) {
        std::cerr << "Invalid token\n";
        return 1;
    }

    for (const auto& device : client.getDevices()) {
        std::cout << device->Label.Get() << "  [" << device->Type.Get() << "]\n";
        for (const auto& id : device->getCapabilityIds()) {
            std::cout << "    " << id << "\n";
        }
    }
    return 0;
}
```

Typical output:

```
Living Room Lamp  [ZIGBEE]
    switch
    switchLevel
    refresh
Lave-linge  [OCF]
    switch
    washerOperatingState
    samsungce.washerOperatingState
    ...
```

From here:

- **Control something** → [Devices & capabilities](device-model.md)
- **React to changes** → [Live updates](live-updates.md)
- **Get a proper token** → [Authentication](authentication.md)

---

## Build options

| Option | Default | Effect |
|---|---|---|
| `BUILD_TESTS` | `ON` | Build the Catch2 unit tests |
| `BUILD_EXAMPLES` | `ON` | Build the programs in `examples/` |
| `BUILD_SHARED_LIBS` | `OFF` | Build as a shared library instead of static |

```bash
cmake -B build -S . -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF \
  -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake
```

---

<div align="center">

[← Documentation home](README.md) · **Next:** [Authentication →](authentication.md)

</div>
