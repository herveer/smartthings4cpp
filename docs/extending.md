[📚 Documentation](README.md) › **Extending the library**

# Extending the library

smartthings4cpp follows one design rule everywhere: **automatic by default,
injectable by interface.** The embedded HTTP server, the token stores and the
webhook verifier are defaults on the owning object, not obligations — when your
app already has an HTTP stack, a settings system, or a crypto library, you
implement a small interface and hand it to `Client`.

| Interface | Default | Replace it when… |
|---|---|---|
| [`IHttpServer`](#ihttpserver) | Embedded cpp-httplib server on `127.0.0.1` | Your app already serves HTTP, or you deploy behind your own reverse proxy |
| [`IStorageProvider`](#istorageprovider) | Windows Credential Manager (secrets) + JSON file (state) | You want a database, another OS keychain, or your own settings store |
| [`IWebhookSignatureVerifier`](#iwebhooksignatureverifier) | Windows CNG verifier | You're on Linux/macOS, or want different crypto/policy |
| [Token refresh callback](#token-refresh-callback) | Wired automatically in OAuth mode | You drive the manual OAuth flow yourself |

All three interfaces are injected through the DI constructor:

```cpp
Client client(config,
    std::make_unique<MyHttpServer>(...),     // IHttpServer
    std::make_unique<MySecretStore>(...),    // IStorageProvider (secrets)
    std::make_unique<MyStateStore>(...));    // IStorageProvider (plain state)
```

…except the verifier, which is swappable at any time via
`client.setWebhookSignatureVerifier(...)`.

---

## IHttpServer

An OAuth `Client` needs two inbound routes: the OAuth redirect (a browser GET
carrying the authorization code) and the webhook (SmartThings POSTs). The
interface abstracts *where those requests come from*; your implementation never
needs to know anything about OAuth or the webhook protocol.

```cpp
class IHttpServer {
public:
    void bind(OAuthCallback oauthCallback, WebhookCallback webhookCallback);
    virtual Result<void> start() = 0;
    virtual void stop() = 0;
    virtual std::string fullOAuthCallbackUri() = 0;
protected:
    void onOAuthReceived(std::string args);                       // call on redirect GET
    WebhookResponse onWebhookReceived(const WebhookRequest& req); // call on webhook POST
};
```

The `Client` calls `bind()` (wiring itself in) and then `start()`. Your job is
to route inbound requests to the two protected forwarders:

```cpp
class MyFrameworkBridge : public IHttpServer {
public:
    Result<void> start() override {
        // Register two routes on your existing server:
        app().get("/oauth/callback", [this](const Req& r) {
            onOAuthReceived(r.target());   // full target: "/oauth/callback?code=...&state=..."
            return htmlPage("You can close this tab.");
        });
        app().post("/webhook", [this](const Req& r) {
            WebhookRequest req;
            req.method  = "POST";
            req.path    = r.target();      // path + query, as received
            req.body    = r.body();        // EXACT bytes - do not re-serialize!
            for (auto& [k, v] : r.headers())
                req.headers[toLower(k)] = v;
            WebhookResponse resp = onWebhookReceived(req);
            return jsonResponse(resp.statusCode, resp.body);
        });
        return {};
    }
    void stop() override { /* unregister/stop as appropriate */ }
    std::string fullOAuthCallbackUri() override {
        return "https://my.example.org/oauth/callback";   // as registered on the app
    }
};
```

Contract details that matter:

- **`fullOAuthCallbackUri()` is the public URL** of the OAuth route — the
  `Client` sends it as `redirect_uri`, so it must exactly match a redirect URI
  registered on your OAuth-In App.
- **Pass the webhook body through byte-for-byte.** The HTTP-Signature covers a
  SHA-256 of the exact bytes; parsing and re-serializing the JSON breaks
  verification. Same for the request line — `path` feeds `(request-target)`.
- **Lowercase header keys** (lookups are case-insensitive anyway, but the
  convention keeps things predictable); join repeated headers with `", "`.
- The `Client` calls back into itself from your server's thread — `stop()` must
  make sure no callback is in flight after it returns (the `Client` destructor
  stops the server before tearing anything else down).

## IStorageProvider

Three methods, string-in/string-out:

```cpp
class IStorageProvider {
public:
    virtual std::optional<std::string> getValue(const std::string& key) = 0;
    virtual bool setValue(const std::string& key, const std::string& value) = 0;
    virtual void removeValue(const std::string& key) = 0;
};
```

The `Client` takes **two** instances with different sensitivity levels:

| Role | Holds | Default |
|---|---|---|
| *Keychain* provider | The OAuth token pair (secrets!) | Windows Credential Manager (`smartthings4cpp/<key>` entries); `keychain.json` file fallback elsewhere |
| *Storage* provider | The installed-app id (plain state) | `storage.json` in the working directory |

Example — backing both with your app's encrypted settings store:

```cpp
class SettingsStorage : public IStorageProvider {
public:
    explicit SettingsStorage(MySettings& s, std::string prefix)
        : _settings(s), _prefix(std::move(prefix)) {}

    std::optional<std::string> getValue(const std::string& key) override {
        std::lock_guard lock(_mutex);
        return _settings.tryRead(_prefix + key);
    }
    bool setValue(const std::string& key, const std::string& value) override {
        std::lock_guard lock(_mutex);
        return _settings.write(_prefix + key, value);
    }
    void removeValue(const std::string& key) override {
        std::lock_guard lock(_mutex);
        _settings.erase(_prefix + key);
    }
private:
    MySettings& _settings;
    std::string _prefix;
    std::mutex _mutex;
};
```

> [!IMPORTANT]
> Implementations must be **thread-safe**: the `Client` touches storage from
> the caller's thread *and* from the HTTP server's thread (token persistence
> happens when the redirect lands).

The built-in pieces are reusable à la carte: `FileStorageProvider` (a
JSON-file store), `makeDefaultStorageProvider(dir)` and
`makeDefaultKeychainProvider(dir)` are all public — mix one default with one
custom implementation freely.

## IWebhookSignatureVerifier

One method:

```cpp
class IWebhookSignatureVerifier {
public:
    virtual Result<void> verify(const WebhookRequest& request) = 0;
};
```

Return `Success` to accept the request; any error `Result` rejects it with
HTTP 401. The most common reason to implement it is **non-Windows crypto** (the
default uses Windows CNG). The pure helpers in `webhook_signature.h` do all the
fiddly parsing, so an OpenSSL version is mostly glue:

```cpp
class OpenSslVerifier : public IWebhookSignatureVerifier {
public:
    Result<void> verify(const WebhookRequest& request) override {
        using namespace http_signature;

        auto auth = request.header("authorization");
        if (!auth) return { ErrorCode::AuthenticationFailed, "no signature" };

        auto params = parseSignatureParams(*auth);          // keyId, algorithm, headers, sig
        if (!params) return { ErrorCode::AuthenticationFailed, "malformed signature" };

        auto signingString = buildSigningString(params->headers, request);
        if (!signingString) return { ErrorCode::AuthenticationFailed, "missing signed header" };

        std::string pem = fetchAndCacheKey(params->keyId);  // GET key.smartthings.com + keyId
        // ... EVP_DigestVerifyInit(SHA-256) over *signingString against the
        // base64-decoded params->signatureBase64, plus a SHA-256(body) check
        // against the Digest header ...
        return ok ? Result<void>{} : Result<void>{ ErrorCode::AuthenticationFailed, "bad signature" };
    }
};

client.setWebhookSignatureVerifier(std::make_unique<OpenSslVerifier>());
```

For **policy** tweaks (replay windows, key sources, test fixtures) you usually
don't need a custom class — the default verifier's
[`WebhookSignatureConfig` and `PublicKeyResolver`](webhook-security.md#configuration)
cover those.

## Token refresh callback

When you manage tokens yourself (manual OAuth flow), teach the `Client` to
recover from access-token expiry — it invokes the callback once on a 401 and
retries the request:

```cpp
client.setTokenRefreshCallback([&]() -> Result<std::string> {
    auto r = authenticator.refresh(myRefreshToken);
    if (!r) return { r.error, r.error_message };
    myRefreshToken = r.value->refreshToken;    // persist the rotated token!
    return Result<std::string>(r.value->accessToken);
});
```

## Testing hooks

Two more seams designed for tests and unusual setups:

- **`client.setBaseUrl(url)`** — point the client at a mock/staging API server
  instead of `https://api.smartthings.com/v1`.
- **`client.registerDevice(device)`** — include a hand-constructed `Device` in
  the polling/dispatch registry.
- **`handleWebhook(rawBody)`** — feed webhook payloads directly (no server, no
  signature), which is how the offline unit tests exercise the whole event
  pipeline.

---

<div align="center">

[← Webhook security](webhook-security.md) · **Next:** [Capability reference →](capabilities.md)

</div>
