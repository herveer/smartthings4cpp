[📚 Documentation](README.md) › **Troubleshooting & FAQ**

# Troubleshooting & FAQ

- [Authentication](#authentication)
- [Live updates & webhooks](#live-updates--webhooks)
- [Building](#building)
- [API behavior](#api-behavior)

---

## Authentication

### My token worked yesterday and returns 401 today

That's the PAT 24-hour expiry: Personal Access Tokens created after
30 December 2024 **expire 24 hours after creation** with no way to renew them.
Generate a new one — or better, [switch to an OAuth-In
App](authentication.md#oauth-recommended), whose tokens refresh automatically.

### I can't find where to create an "API Access App" in the Developer Console

You can't — the Console menus for the new app model aren't live yet. Create the
app with the **SmartThings CLI** instead: `smartthings apps:create` →
**OAuth-In App** (confirmed working by SmartThings developer support). See
[Authentication](authentication.md#step-1--register-your-oauth-in-app).

### My redirect URI is rejected at registration

SmartThings documents redirect URIs as HTTPS-only. Register your tunnel URL
(`https://your-domain.ngrok-free.dev/oauth/callback`) rather than a
`http://localhost:…` one. In practice plain-HTTP localhost URIs have been
accepted for personal apps, but the tunnel form always works — and with the
embedded server, the redirect passes through the tunnel anyway.

### `authenticate()` returns an error immediately

Check the message on the `Result`: the usual causes are the embedded server
failing to start (**port already in use** — pick another, or pass `0` to let it
choose) or calling OAuth methods on a PAT-constructed client
(`"not an OAuth client"`).

### The browser opened but nothing completes

The redirect never reached the embedded server. Verify the tunnel is running,
and that the registered redirect URI is exactly
`externalUri + oauthCallbackRoute` for the values you passed to the `Client`
constructor — the match must be character-exact.

### Do I have to re-authorize every ~24 hours?

No. The access token expires in ~24 h, but the client refreshes it silently
with the refresh token. Only if the app stays **unused for ~29 days** does the
refresh token itself lapse — then the browser consent runs once more.

## Live updates & webhooks

### No events arrive

Work down this checklist:

1. **Tunnel up?** `ngrok http --url=<your-domain> <port>` running, and the
   domain matches the `externalUri` you passed to `Client`.
2. **Target URL right?** The app's Target URL must be
   `externalUri + webhookCallbackRoute` (e.g.
   `https://your-domain.ngrok-free.dev/webhook`). Check with
   `smartthings apps <id>`; SmartThings must have completed its
   `CONFIRMATION` handshake against it.
3. **Requests arriving?** Open ngrok's inspector at <http://127.0.0.1:4040>.
   Nothing there → cloud-side registration problem. Requests with **401**
   responses → signature verification is rejecting them (next question).
4. **Subscriptions exist?** `client.listSubscriptions()` should show one entry
   per device you hold. Empty → authentication or installed-app id trouble;
   check `client.installedAppId()` is non-empty.
5. **Devices alive?** Subscriptions are tied to `Device` lifetimes — if the
   `shared_ptr`s went out of scope, the subscriptions were deleted.

### Every webhook is answered 401

Signature verification is failing. On **Linux/macOS this is expected** with the
default verifier — its crypto backend is Windows CNG only; inject your own (or
disable verification while debugging):
[Webhook security](webhook-security.md#platform-support). On Windows, make sure
your `IHttpServer` (if custom) passes the **exact body bytes** and headers
through — re-serializing the JSON breaks the signed digest.

### Events arrive but `PropertyChanged` doesn't fire

The event dispatches onto live `Device` objects for that device id — make sure
you're still holding the `shared_ptr<Device>` and the `ScopedSubscription`.
Also remember events fire only when the value **actually changed**.

### I get `RateLimited` errors

SmartThings caps device status reads at **12/min per device** and rate-limits
subscription writes. The automatic paths respect this (polling interval clamps
to ≥ 5 s; subscriptions are created once and adopted across runs) — if you're
calling `pollNow()` or `subscribeTo()` in a loop, back off.

## Building

### CMake can't find `ReactiveLitepp`

`reactivelitepp` comes from the `herve-er/vcpkg-registery` registry pinned in
`vcpkg-configuration.json`, with a backup port in `overlay-ports/`. Make sure
you configured with the **vcpkg toolchain file** so manifest mode runs; a
plain `cmake -B build` without it will find none of the dependencies.

### `cmake` / `ninja` not found on Windows

Visual Studio ships its own copies that aren't on the global `PATH`. Build
from a *Developer PowerShell/Command Prompt* (or run `VsDevCmd.bat` first).

### Cannot move or copy a `Client`

By design: `Client` owns a background thread that captures `this`. Keep it in
place — behind a `std::unique_ptr<Client>` if you need heap or container
storage.

## API behavior

### Attribute getters return empty values

Getters read cached state, filled on the first `getComponent()` /
`getCapability()` call (which auto-fetches status). If you constructed a
capability by hand in a test, or the fetch failed (check
`device->HasBeenRefreshed.Get()`), the cache is empty — call
`device->refreshStatus()` and inspect its `Result`.

### `getDevices()` returns an empty vector — error or empty account?

Both look the same by design (collection getters don't return `Result`s).
Call `client.validateAuthentication()` to distinguish: it pinpoints
authentication/network failures with a proper `ErrorCode`.

### A capability on my device has no typed class

It materializes as [`UnknownCapability`](device-model.md#unknown-capabilities)
— raw JSON status still available, events still fire. To promote it,
[regenerate the capability classes](capabilities.md#regenerating) with your
PAT; the generator picks up whatever schemas your devices expose.

### Which thread do my handlers run on?

The update-delivery thread: the polling thread (polling mode) or the embedded
server's thread (OAuth mode). Synchronize anything shared; marshal to your UI
thread yourself. See [Threading](live-updates.md#threading).

---

Still stuck? [Open an issue](https://github.com/herveer/smartthings4cpp/issues)
with the failing `Result`'s `error`/`error_message` and (for webhook trouble) a
screenshot of the ngrok inspector.

---

<div align="center">

[← Examples tour](examples.md) · [Documentation home](README.md)

</div>
