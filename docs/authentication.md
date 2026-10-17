[📚 Documentation](README.md) › **Authentication**

# Authentication

Every request to the SmartThings API carries an OAuth 2.0 Bearer token. There
are two ways to get one, and the choice shapes everything downstream:

| | 🌐 OAuth-In App | 🔑 Personal Access Token |
|---|---|---|
| Lifetime | Access token ~24 h, **refreshed automatically** | **24 hours, then dead** — no renewal |
| Live updates | Real cloud **push** (webhook events) | Polling only (a PAT cannot subscribe to events) |
| Setup | Register an app once (CLI, ~15 min) | Generate on a web page (~1 min) |
| Best for | Anything real | Quick experiments |

- [OAuth (recommended)](#oauth-recommended)
  - [Step 1 — Register your OAuth-In App](#step-1--register-your-oauth-in-app)
  - [Step 2 — The autonomous Client](#step-2--the-autonomous-client)
  - [Token persistence](#token-persistence)
  - [The manual OAuth flow](#the-manual-oauth-flow)
  - [Bring your own tokens](#bring-your-own-tokens)
- [Personal Access Tokens (discouraged)](#personal-access-tokens-discouraged)
- [Transparent token refresh](#transparent-token-refresh)
- [Security notes](#security-notes)

---

## OAuth (recommended)

With your own **OAuth-In App** ("API Access App"), SmartThings issues a
refreshable token pair *and* delivers real push events. It is the same
self-service model as a PAT — every developer registers their own app and gets
their own `clientId`/`clientSecret`; smartthings4cpp does not embed or share
any secret of its own.

### Step 1 — Register your OAuth-In App

> [!IMPORTANT]
> **Create the app with the SmartThings CLI, not the Developer Console.**
> Samsung is mid-migration to the new "API Access App" model and the Console
> menus for it are not live yet, but the CLI path works end to end — confirmed
> by SmartThings developer support.

1. Install the [SmartThings CLI](https://github.com/SmartThingsCommunity/smartthings-cli).
2. Run:

   ```bash
   smartthings apps:create
   ```

   and pick **OAuth-In App**. When prompted:
   - **Scopes:** `r:devices:*`, `x:devices:*`, `r:locations:*`
   - **Redirect URI:** where the browser lands after consent. For the
     embedded-server setup below this is
     `https://<your-public-host><oauth-route>` — e.g.
     `https://your-domain.ngrok-free.dev/oauth/callback` (see the
     [ngrok walkthrough](live-updates.md#the-ngrok-walkthrough) for getting a
     stable public host). You can add or edit redirect URIs later with
     `smartthings apps:oauth:update`.
3. Save the `clientId` and `clientSecret` it prints — the secret is shown once.

> [!TIP]
> SmartThings' docs say redirect URIs must be HTTPS. A tunnel domain satisfies
> that; in practice a plain `http://localhost:<port>/...` URI has also been
> accepted for self-registered personal apps. If yours is rejected, use the
> tunnel URL.

### Step 2 — The autonomous Client

The OAuth-mode `Client` runs the whole story by itself: it embeds a small local
HTTP server for both the OAuth redirect and the SmartThings webhook, drives the
authorization flow, persists tokens across runs, and manages event
subscriptions automatically. **The single manual step is opening a browser** —
the library never does that itself:

```cpp
#include <smartthings4cpp/smartthings4cpp.h>

using namespace smartthings4cpp;

oauth2::OAuth2Config config;
config.clientId     = "...";   // from `smartthings apps:create`
config.clientSecret = "...";

Client client(config,
    9753,                                    // local port of the embedded server
    "/oauth/callback",                       // OAuth redirect route
    "/webhook",                              // webhook route
    "https://your-domain.ngrok-free.dev",    // public HTTPS base URL (the tunnel)
    "./st4cpp-data");                        // working dir for persisted state

// The one manual step: present the consent URL when asked.
client.openBrowserRequested += [](std::string url) {
    std::cout << "Open this URL to authorize:\n" << url << "\n";
};

client.authenticate();                                  // silent when a stored token works
client.waitForAuthentication(std::chrono::minutes(5));  // only blocks on the first run
```

What each constructor argument means:

| Argument | Meaning |
|---|---|
| `config` | Your app's `clientId`/`clientSecret` (+ scopes, defaulting to the three above) |
| `port` | Local port the embedded server listens on (`0` = pick a free one) |
| `oauthCallbackRoute` | Path of the OAuth redirect route |
| `webhookCallbackRoute` | Path SmartThings POSTs events to |
| `externalUri` | The **public HTTPS base URL** under which both routes are reachable from the internet — `externalUri + oauthCallbackRoute` is sent as the `redirect_uri` and must exactly match one registered on your app |
| `workingDir` | Directory for persisted non-secret state (created if missing) |

`authenticate()` tries, in order: the stored access token, the stored refresh
token (rotating and persisting the new pair), and only then the interactive
flow — firing `openBrowserRequested` and returning immediately. Observe the
outcome either by blocking in `waitForAuthentication()` or by subscribing to
`authenticationCompleted`:

```cpp
client.authenticationCompleted += [](Result<void> outcome) {
    if (!outcome) std::cerr << "auth failed: " << outcome.error_message << "\n";
};
```

> [!NOTE]
> The embedded server listens on `127.0.0.1` in plain HTTP. TLS terminates at
> your tunnel/reverse proxy. Setting up that tunnel (and the app's Target URL
> for events) is covered step by step in the
> [ngrok walkthrough](live-updates.md#the-ngrok-walkthrough).

### Token persistence

Tokens survive restarts, so the browser typically opens **only the very first
run**:

- **Secrets** (the access/refresh token pair) go to the OS credential store
  where one is implemented — on **Windows**, the Credential Manager (entries
  named `smartthings4cpp/<key>`, visible in the *Credential Manager* control
  panel). On other platforms they currently fall back to a `keychain.json`
  file in the working directory.
- **Non-secrets** (the installed-app id) go to `storage.json` in the working
  directory.

Both stores are behind the [`IStorageProvider`](extending.md#istorageprovider)
interface — inject your own to use a different keychain, a database, or your
app's settings system.

> [!NOTE]
> Refresh tokens expire after ~29 days of *disuse*; every use rotates and
> re-persists the pair, resetting that clock. An app run at least monthly never
> sees a browser again.

### The manual OAuth flow

If you don't want the embedded server — a GUI app with its own webview, a
mobile deep link, a paste-the-URL-back CLI — drive the protocol yourself with
`oauth2::OAuth2Authenticator`. It does the two things that are protocol rather
than UX (build the authorize URL; exchange a code or refresh token) and has no
opinion about anything else:

```cpp
using namespace smartthings4cpp;
using namespace smartthings4cpp::oauth2;

OAuth2Config config;
config.clientId     = "...";
config.clientSecret = "...";
OAuth2Authenticator authenticator(config);

// 1. Present this URL however you like (browser, webview, print it, ...).
std::string url = authenticator.buildAuthorizeUrl("https://my.example.org/callback");

// 2. Hand back whatever the redirect produced - the full redirect URL or a
//    bare code. CSRF state is validated for you.
Result<OAuth2Token> token = authenticator.completeAuthorization(redirect_result);

// 3. Use the token with a plain Client (polling mode - no embedded server).
Client client(token.value->accessToken);
```

Refresh later with `authenticator.refresh(refreshToken)` — and always persist
the *new* refresh token that comes back, because SmartThings may rotate it.
The [`oauth2_authentication` example](examples.md#oauth2_authentication) is a
complete working version of this flow, including persistence to a JSON file.

### Bring your own tokens

The manual flow above leaves you with a plain polling `Client`. If your
application runs OAuth itself (it builds the authorize URL, receives the
redirect, exchanges the code, keeps and refreshes the tokens) **and** wants
real push events, use the *external-tokens* OAuth mode instead. It does
everything the autonomous OAuth client does except the OAuth flow:

| The library still does | Your application does |
|---|---|
| Webhook handling: PING, CONFIRMATION, INSTALL/UPDATE, EVENT, UNINSTALL | The OAuth flow: authorize URL, redirect, code exchange |
| [HTTP-Signature verification](webhook-security.md) (on by default) | Storing and refreshing tokens |
| Installed-app id capture (from `setAccessToken()` or the webhook) | Receiving the webhook POST and handing it over (`IHttpServer`) |
| Automatic per-device subscriptions, adopted across runs | Calling `setAccessToken()` with every new token |
| Device events dispatched onto your `Device` objects | Refreshing when told a token was rejected |

No client secret, no keychain and no polling are involved:

```cpp
Client client(ExternalTokens{},
    std::make_unique<MyGatewayServer>(),   // IHttpServer (may be null, see below)
    std::make_unique<MyStateStore>());     // IStorageProvider for the installed-app id

client.accessTokenRejected += [&](std::string rejected) {
    myOAuth.refreshAsync();                // never block here waiting for it
};
client.authenticationCompleted += [](Result<void>) { /* first token arrived */ };

// At startup, and again after every refresh:
client.setAccessToken(token.access, token.installedAppId);
```

The SmartThings token response carries `installed_app_id`; pass it as the
second argument so subscriptions can start before any webhook arrives (an empty
value keeps the current id).

How each piece behaves in this mode:

- **`setAccessToken(token, installedAppId)`** is thread-safe and never blocks:
  call it from any thread, as often as you like. The token is used by the very
  next request. Persisting the id, flushing the subscriptions that were waiting
  for a token, and firing `authenticationCompleted` (once per sign-in, not for
  each refreshed token) happen on the client's internal worker thread.
- **An empty token signs the client out.** Calls then fail with
  `ErrorCode::AuthenticationRequired` (getters return empty results); nothing is
  deleted: the installed-app id, the subscriptions and your `Device` objects
  stay, and pushed events keep flowing to them. The next token signs it back in.
- **`accessTokenRejected`** fires when the API answers 401 for the current
  token, once per token (setting a token again re-arms it), on the worker
  thread, so the handler may call back into the client. The library never
  waits for a new token: the request that got the 401 returns its error at
  once (`ErrorCode::AuthenticationFailed` for commands). If you already
  supplied a newer token while that request was in flight, it is retried once
  with it. Subscriptions that failed are retried when the next token arrives.
- **`authenticate()`** runs nothing and never fires `openBrowserRequested`: it
  returns `true` if a token is set, `false` if not. **`waitForAuthentication()`**
  never waits: success if a token is set, `AuthenticationRequired` otherwise.
- **The `IHttpServer`** is bound and started as usual, but only its webhook
  route is used; `fullOAuthCallbackUri()` is never called. Pass `nullptr` if
  you deliver webhook requests yourself through
  `handleWebhook(const WebhookRequest&)`, which verifies the signature too.

> [!IMPORTANT]
> Because no call ever blocks waiting for a token, you can safely call the
> client from the same thread that will produce the next token. Do refresh
> asynchronously, though: a handler that blocks holds up the worker's other
> jobs, and must never wait on the thread that destroys the `Client`.

---

## Personal Access Tokens (discouraged)

> [!WARNING]
> PATs created after 30 December 2024 **expire 24 hours after creation** and
> cannot be renewed. A PAT also **cannot subscribe to events**, so live updates
> fall back to polling. Use a PAT to try the library; use OAuth for anything
> that outlives a day.

1. Go to <https://account.smartthings.com/tokens>.
2. Create a token with the **`r:devices:*`**, **`x:devices:*`** (needed to send
   commands) and **`r:locations:*`** scopes.
3. Copy it immediately — it is shown only once.

```cpp
Client client("your-personal-access-token");   // polls automatically
if (!client.validateAuthentication()) {
    std::cerr << "Token invalid or expired\n"; // after 24h: this
}
```

`validateAuthentication()` performs one lightweight authenticated request and
tells you whether the token is live — cheap insurance at startup given the
24-hour cliff.

The examples read the PAT from the `SMARTTHINGS_TOKEN` environment variable, a
`smartthings_token.txt` file (gitignored), or an interactive prompt.

---

## Transparent token refresh

Any `Client` — PAT-constructed or manual-OAuth — can self-heal past access
token expiry instead of failing with 401s. Give it a callback that produces a
fresh token; on the first 401 the `Client` invokes it once and retries the
request, invisibly to your code:

```cpp
client.setTokenRefreshCallback([&]() -> Result<std::string> {
    auto refreshed = authenticator.refresh(storedRefreshToken);
    if (!refreshed) return { refreshed.error, refreshed.error_message };
    storedRefreshToken = refreshed.value->refreshToken;   // rotation!
    return Result<std::string>(refreshed.value->accessToken);
});
```

The autonomous OAuth `Client` wires this internally — you only need it when
driving the manual flow.

---

## Security notes

- **Never commit tokens or client secrets.** The repository's `.gitignore`
  already excludes `smartthings_token.txt`, `*.token` and `*.key`; keep your
  own secrets in environment variables or the OS keychain.
- Scope tokens minimally: drop `x:devices:*` if you only read state.
- On Windows the OAuth token pair lives in the Credential Manager, not on disk.
  On other platforms, prefer [injecting a real keychain
  backend](extending.md#istorageprovider) over the file fallback for
  production.
- Inbound webhooks are cryptographically verified by default — see
  [Webhook security](webhook-security.md).

---

<div align="center">

[← Getting started](getting-started.md) · **Next:** [Devices & capabilities →](device-model.md)

</div>
