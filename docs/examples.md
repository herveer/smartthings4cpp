[📚 Documentation](README.md) › **Examples tour**

# Examples tour

Seven runnable programs in [`examples/`](../examples/) cover the library's
whole surface. They are built by default (`BUILD_EXAMPLES=ON`) and land in
`build/examples/`.

| Example | Demonstrates | Auth | Scopes needed |
|---|---|---|---|
| [`authentication`](#authentication) | Token setup & validation | PAT | `r:devices:*` |
| [`device_listing`](#device_listing) | Discovery: devices, locations, rooms, capabilities | PAT | `r:devices:*`, `r:locations:*` |
| [`device_control`](#device_control) | Live status + real commands | PAT | + `x:devices:*` |
| [`full_discovery`](#full_discovery) | Every attribute & command of every device | PAT | `r:devices:*` |
| [`live_updates`](#live_updates) | Change notifications via background polling | PAT | `r:devices:*` |
| [`oauth2_authentication`](#oauth2_authentication) | The manual OAuth 2.0 flow | OAuth | — |
| [`event_updates`](#event_updates) | **Real push**: autonomous OAuth + webhook | OAuth | — |

**Credentials:** the PAT examples read the token from the `SMARTTHINGS_TOKEN`
environment variable, a `smartthings_token.txt` file next to the executable
(gitignored), or an interactive prompt. The OAuth examples read
`SMARTTHINGS_CLIENT_ID` / `SMARTTHINGS_CLIENT_SECRET` the same way.

```powershell
# Windows (PowerShell)
$env:SMARTTHINGS_TOKEN = "<your-pat>"
```

```bash
# Linux / macOS
export SMARTTHINGS_TOKEN="<your-pat>"
```

---

## `authentication`

The smallest possible program: configure a token, `validateAuthentication()`,
report the verdict. Run it first to confirm your token and scopes work.

```bash
./build/examples/authentication
```

## `device_listing`

Connects to your account and prints every device grouped by location, with
room, metadata and capability list:

```bash
./build/examples/device_listing
```

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

## `device_control`

Fetches a device's live status, prints typed attribute values, then sends a
real **self-restoring** command — it nudges and restores the volume, or toggles
a switch twice, so your home ends up exactly as it started. Target a specific
device with `SMARTTHINGS_DEVICE_ID`, otherwise it picks the first controllable
one:

```bash
$env:SMARTTHINGS_DEVICE_ID = "<device-uuid>"   # optional
./build/examples/device_control                # needs x:devices:* scope
```

## `full_discovery`

Read-only deep dump: **every device, every component, every capability, every
attribute value, every command name** — including the Samsung-proprietary
capabilities. Great for discovering what your hardware actually exposes:

```bash
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
    samsungce.washerOperatingState:
        attributes:
            operatingState: ready
            progress: 1 %
            supportedOperatingStates: [ready, running, paused]
        commands:
            resume
            ...
```

(Capabilities on non-`main` components are shown as `<component>/<capability>`.)

## `live_updates`

Watches every `contactSensor`/`switch` device (or one picked via
`SMARTTHINGS_DEVICE_ID`) and prints each attribute change as the client's
automatic background polling detects it. Open a door or flip a switch in the
SmartThings app while it runs:

```bash
./build/examples/live_updates
```

## `oauth2_authentication`

The [manual OAuth flow](authentication.md#the-manual-oauth-flow), piece by
piece: builds the authorize URL, prompts you to paste the redirect back,
exchanges the code, persists the token to `oauth2_token.json`, and refreshes it
automatically on later runs. Educational — for real apps prefer the autonomous
client shown by `event_updates`:

```bash
$env:SMARTTHINGS_CLIENT_ID     = "<client-id>"
$env:SMARTTHINGS_CLIENT_SECRET = "<client-secret>"
./build/examples/oauth2_authentication
```

## `event_updates`

The full autonomous push story — the code behind the
[ngrok walkthrough](live-updates.md#the-ngrok-walkthrough). It authenticates
itself (printing the authorize URL on the first run only), receives verified
events on its embedded server, and prints every change SmartThings pushes:

1. Complete the [one-time app + tunnel setup](live-updates.md#the-ngrok-walkthrough).
2. Edit the constants at the top of
   [`event_updates.cpp`](../examples/event_updates.cpp) — the listen port and,
   importantly, the **`externalUri`** passed to the `Client` constructor, which
   must be *your* tunnel domain — and rebuild.
3. Run:

```bash
$env:SMARTTHINGS_CLIENT_ID     = "<client-id>"
$env:SMARTTHINGS_CLIENT_SECRET = "<client-secret>"
ngrok http --url=your-domain.ngrok-free.dev 9057    # in another terminal
./build/examples/event_updates
```

```
Watching 3 device(s) - subscriptions created automatically:
  - Living Room Lamp (1111...)
  [push] Living Room Lamp / main / switch: SwitchValue changed, new value = on
```

Tokens persist across runs (Windows Credential Manager / file), so only the
very first run opens a browser.

---

<div align="center">

[← Capability reference](capabilities.md) · **Next:** [Troubleshooting & FAQ →](troubleshooting.md)

</div>
