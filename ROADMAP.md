# smartthings4cpp Development Roadmap

This document outlines the development plan for **smartthings4cpp**, a C++ library
for the Samsung SmartThings public API. It is a living document and will be updated
as the project progresses.

## Project Vision

Provide a lightweight, cross-platform C++ library that gives developers an intuitive,
reactive interface to the SmartThings cloud — mirroring the architecture and style of
the sibling [hue4cpp](https://github.com/herveer/hue4cpp) library, and adopting the
reactive pattern of [reactivelitepp](https://github.com/herveer/reactivelitepp).

## How SmartThings differs from Hue

| | Philips Hue (hue4cpp) | SmartThings (smartthings4cpp) |
|---|---|---|
| Topology | Local bridge on the LAN | Cloud API (`api.smartthings.com`) |
| Discovery | mDNS / N-UPnP | None — account is reached directly |
| Auth | Link-button → application key | OAuth 2.0 Bearer token (PAT in iteration 1) |
| TLS | Self-signed (verification off) | Public certificates (verification on) |
| Entry point | `Bridge` | `Client` |
| Resources | Lights, sensors, devices | Devices → components → capabilities |
| Real-time | Server-Sent Events | Webhooks / subscriptions (future) |

---

## Phase 1: Authentication & Device Discovery ✅ CURRENT ITERATION

**Goal:** Connect to a SmartThings account and list every device with its capabilities.

**Tasks:**
- [x] Core infrastructure (HTTP client, JSON utilities, error/result types)
- [x] `Client` entry point with PAT-based Bearer-token authentication
- [x] Token validation against the cloud (`GET /v1/devices?max=1`)
- [x] Device discovery with pagination (`GET /v1/devices`, follow `_links.next`)
- [x] `Device` as a reactive `ObservableObject` with metadata properties
- [x] Component & capability parsing (`getComponents`, `getCapabilityIds`, `hasCapability`)
- [x] Location & room discovery (`GET /v1/locations`, `/locations/{id}/rooms`)
- [x] Offline unit tests (Catch2) for parsing, types, and client state
- [x] Examples: `device_listing`, `authentication`

**Deliverable:** run the `device_listing` demo, connect to your account, and see all
your devices and their capabilities.

---

## Phase 2: Device Control 🔜 NEXT

**Goal:** Read live device state and send commands.

**Tasks:**
- [ ] Device status (`GET /v1/devices/{id}/status`) → attribute values per capability
- [ ] Command execution (`POST /v1/devices/{id}/commands`)
- [ ] Capability-aware reactive properties on `Device` (e.g. `switch`, `switchLevel`,
      `colorControl`) using `Property<T>` with getter/setter that call the API
- [ ] Convenience helpers for common capabilities
- [ ] Tests and a `device_control` example

**Design seams already in place:** `Device` holds a `Client*` back-pointer, and the
`Capability` type is ready to carry attribute values.

---

## Phase 3: Real-time Updates 🔮

- [ ] SmartThings subscriptions / webhooks (or SSE where available)
- [ ] A state-manager layer that fires `ReactiveLitepp::Event`s, like hue4cpp's
      `StateManager`, so `Device` properties update live
- [ ] Event-driven examples

---

## Phase 4: Production Authentication 🔮

- [ ] OAuth 2.0 authorization-code flow (authorize URL, code→token exchange, refresh)
- [ ] Token storage via OS keychains (Windows Credential Manager, macOS Keychain,
      Linux libsecret)
- [ ] SmartApp registration guidance

---

## Phase 5: Breadth & Polish 🔮

- [ ] Scenes (`GET /v1/scenes`, execute scene)
- [ ] Rules
- [ ] Capability presentation metadata
- [ ] CI/CD, Doxygen docs, broader platform testing

---

## Technology Stack

- **Build System**: CMake 3.16+
- **Package Manager**: vcpkg
- **HTTP Client**: cpr (libcurl wrapper)
- **JSON Library**: nlohmann-json
- **Reactive**: reactivelitepp
- **Testing**: Catch2
- **C++ Standard**: C++17

---

**Last Updated**: 2026-06-29
**Current Phase**: Phase 1 — Authentication & Device Discovery ✅
**Next Step**: Phase 2 — Device Control
