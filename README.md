<div align="center">

# smartthings4cpp

**A modern, reactive C++ library for the Samsung SmartThings cloud.**

Discover your devices, read their live state, send commands, and get real push
updates — with typed capability classes and zero boilerplate.

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.16%2B-064F8C.svg)](https://cmake.org/)
[![vcpkg](https://img.shields.io/badge/deps-vcpkg-yellow.svg)](https://vcpkg.io/)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Platforms](https://img.shields.io/badge/platforms-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](#requirements)

[Getting started](docs/getting-started.md) ·
[Documentation](docs/README.md) ·
[Examples](docs/examples.md) ·
[Capability reference](docs/capabilities.md)

</div>

---

## At a glance

```cpp
#include <smartthings4cpp/smartthings4cpp.h>

using namespace smartthings4cpp;
using namespace smartthings4cpp::standard;

int main() {
    Client client("your-token");            // polls devices in the background

    for (auto& device : client.getDevices()) {
        if (auto* sw = device->getCapability<Switch>()) {
            std::cout << device->Label.Get() << " is " << sw->SwitchValue.Get() << "\n";
            sw->on();                       // POST a real command
        }
    }
}
```

And for live state, you subscribe once and react — whether updates arrive by
background polling (PAT) or by real cloud push (OAuth), the event is the same:

```cpp
auto sub = device->PropertyChanged.SubscribeScoped(
    [](ObservableObject& sender, PropertyChangedArgs args) {
        std::cout << args.PropertyName() << " changed\n";
    });
```

## Why smartthings4cpp?

| | |
|---|---|
| 🧩 **143 typed capability classes** | Every capability your devices expose — 36 standard (`Switch`, `AudioVolume`, `ContactSensor`, …) plus 107 Samsung-proprietary ones (`samsungce::*`, `samsungvd::*`, `custom::*`, …) — as real C++ classes with typed getters and command methods. All generated from the live API schemas; unknown ids gracefully fall back to a generic capability. |
| 🔄 **Reactive by design** | `Device` and every `Capability` are observable objects. Subscribe to `PropertyChanged` at whatever level suits you — capability, component, or whole device — and only *actual* value changes fire. |
| 📡 **Live updates, zero wiring** | A PAT client automatically polls every device it hands out, deduplicated and rate-limit-aware. An OAuth client receives **real push events** instead — no polling at all. |
| 🔐 **Autonomous OAuth** | The OAuth client embeds its own local HTTP server for the redirect *and* the webhook, runs the authorization flow itself, and persists tokens (Windows Credential Manager) so only the very first run opens a browser. |
| 🔏 **Verified webhooks** | Every inbound webhook's HTTP-Signature is cryptographically checked against SmartThings' published public key before the event is acted on — on by default. |
| 🔧 **Escape hatches everywhere** | Everything automatic is also injectable: bring your own `IHttpServer`, `IStorageProvider`, or `IWebhookSignatureVerifier` when the defaults don't fit. |

## Bring your own tokens

Already running OAuth yourself — your own authorize URL, redirect handler,
code exchange and refresh? Construct the client with `ExternalTokens{}` and
hand it each access token. It keeps everything else the OAuth client does:
webhook handling (PING / CONFIRMATION / EVENT, HTTP-Signature verification,
installed-app id capture), automatic per-device subscriptions and pushed device
events. It never stores, refreshes or waits for a token, and needs no client
secret and no keychain.

```cpp
Client client(ExternalTokens{},
    std::make_unique<MyGatewayServer>(),   // IHttpServer: your webhook route (its OAuth route is unused)
    std::make_unique<MyStateStore>());     // IStorageProvider: plain state (installed-app id)

// A 401 for the current token: refresh asynchronously, then hand the new one in.
client.accessTokenRejected += [&](std::string rejected) { myOAuth.refreshAsync(); };

// At startup and after every refresh - from any thread, never blocks:
client.setAccessToken(token.access, token.installedAppId);

client.setAccessToken("");                 // signed out: calls fail, nothing is deleted
```

`setAccessToken()` adopts the token at once; pending subscriptions are flushed
and `authenticationCompleted` fires on the client's own worker thread, never
on yours. The request that got a 401 returns its error right away, and
`accessTokenRejected` fires once per rejected token. Details in
[Authentication](docs/authentication.md#bring-your-own-tokens).

## Documentation

The full documentation lives in [`docs/`](docs/README.md):

| Guide | What it covers |
|---|---|
| 📦 [Getting started](docs/getting-started.md) | Requirements, building with CMake + vcpkg, using the library in your own project, your first program |
| 🔑 [Authentication](docs/authentication.md) | OAuth (recommended), bringing your own OAuth tokens, and Personal Access Tokens (discouraged — they expire after 24 h), token persistence, transparent refresh |
| 🏠 [Devices & capabilities](docs/device-model.md) | The `Client → Device → Component → Capability` model, typed capability lookup, reactive properties, commands, error handling |
| ⚡ [Live updates](docs/live-updates.md) | Background polling, real push over webhooks, and a complete **ngrok walkthrough** from zero to pushed events |
| 🔏 [Webhook security](docs/webhook-security.md) | How inbound webhooks are cryptographically verified, and how to customize it |
| 🧰 [Extending the library](docs/extending.md) | Implementing your own `IHttpServer`, `IStorageProvider`, and `IWebhookSignatureVerifier` |
| 🧩 [Capability reference](docs/capabilities.md) | All 143 typed capability classes, their namespaces, and how to regenerate them |
| 🚀 [Examples tour](docs/examples.md) | Every runnable example in `examples/`, what it demonstrates, and how to run it |
| 🩺 [Troubleshooting & FAQ](docs/troubleshooting.md) | Expired PATs, missing events, OAuth app creation, rate limits, platform notes |

## Quick start

```bash
git clone https://github.com/herveer/smartthings4cpp.git
cd smartthings4cpp

cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build build

# offline unit tests — no token or network needed
ctest --test-dir build --output-on-failure

# see your own devices (needs a token — see docs/authentication.md)
./build/examples/device_listing
```

Head to [Getting started](docs/getting-started.md) for the full setup, including
how to consume the library from your own CMake project.

## Examples

Seven runnable programs in [`examples/`](examples/) cover the whole surface,
from listing devices to fully autonomous OAuth push:

| Example | Shows |
|---|---|
| [`device_listing`](examples/device_listing.cpp) | Discover every device, with locations, rooms and capabilities |
| [`device_control`](examples/device_control.cpp) | Read live status and send a real (self-restoring) command |
| [`full_discovery`](examples/full_discovery.cpp) | Dump every attribute value and command of every device |
| [`live_updates`](examples/live_updates.cpp) | Watch state changes arrive via automatic background polling |
| [`event_updates`](examples/event_updates.cpp) | Real push: autonomous OAuth + embedded webhook server |
| [`authentication`](examples/authentication.cpp) | Token validation basics |
| [`oauth2_authentication`](examples/oauth2_authentication.cpp) | The manual OAuth 2.0 flow, piece by piece |

See the [examples tour](docs/examples.md) for run instructions and sample output.

## Requirements

- **C++17** compiler — GCC 8+, Clang 7+, MSVC 2019+
- **CMake** 3.16+
- **vcpkg** for dependencies: [cpr](https://github.com/libcpr/cpr),
  [cpp-httplib](https://github.com/yhirose/cpp-httplib),
  [nlohmann-json](https://github.com/nlohmann/json),
  [reactivelitepp](https://github.com/herveer/reactivelitepp),
  [Catch2](https://github.com/catchorg/Catch2) (tests)

> [!NOTE]
> Webhook signature verification uses the native Windows crypto API (CNG). On
> Linux/macOS, inject your own verifier — see
> [Webhook security](docs/webhook-security.md).

## Project structure

```
smartthings4cpp/
├── include/smartthings4cpp/    Public headers (client, device, capabilities, oauth2, …)
├── src/                        Implementation
├── examples/                   Seven runnable example programs
├── tests/                      Offline Catch2 unit tests
├── tools/                      Capability code generator (see tools/README.md)
├── docs/                       📚 The documentation
└── overlay-ports/              Backup vcpkg port for reactivelitepp
```

## License

MIT — see [LICENSE](LICENSE).

---

<div align="center">
<sub>
This library is not affiliated with or endorsed by Samsung or SmartThings.
All trademarks are property of their respective owners.
</sub>
</div>
