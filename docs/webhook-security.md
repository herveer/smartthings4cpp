[📚 Documentation](README.md) › **Webhook security**

# Webhook security

A webhook endpoint is a door to the internet: anyone who discovers your public
URL can POST to it. smartthings4cpp closes that door by **cryptographically
verifying every inbound webhook before acting on it** — on by default in OAuth
mode, with a clean interface to customize or replace it.

- [What SmartThings signs](#what-smartthings-signs)
- [What the default verifier checks](#what-the-default-verifier-checks)
- [Configuration](#configuration)
- [Platform support](#platform-support)
- [Replacing or disabling verification](#replacing-or-disabling-verification)
- [Testing with a key resolver](#testing-with-a-key-resolver)

---

## What SmartThings signs

Every request SmartThings POSTs to a webhook carries an **HTTP Signature**
(IETF *Signing HTTP Messages*, draft-cavage-03) in its `Authorization` header:

```http
Authorization: Signature keyId="/pl/useast1/<uuid>",
    headers="(request-target) digest date",
    algorithm="rsa-sha256",
    signature="<base64 RSA-SHA256 signature>"
```

The signature covers a canonical string assembled from the listed headers —
including the synthetic `(request-target)` (method + path) and the `Digest`
header, which is a SHA-256 hash of the exact body bytes. Verifying it proves
two things at once:

1. the request genuinely came from SmartThings (only they hold the private
   key), and
2. neither the request line, the headers, nor the body were tampered with in
   transit.

The public key rotates; its current id travels in every signature (`keyId`) and
the matching X.509 certificate is published at
`https://key.smartthings.com<keyId>`.

## What the default verifier checks

`Client` in OAuth mode installs the default verifier automatically. For every
webhook POST it:

1. parses the `Authorization: Signature …` header (rejecting requests where it
   is absent or malformed);
2. fetches SmartThings' public key for the signature's `keyId` from the key
   server (cached — see below);
3. rebuilds the canonical signing string from the *exact* request bytes and
   verifies the RSA-SHA256 signature;
4. independently hashes the body and matches it against the signed `Digest`.

Any failure → the request is answered with **HTTP 401** and never reaches the
event pipeline. This was validated against real captured SmartThings traffic.

## Configuration

Build a customized default verifier with `WebhookSignatureConfig` and install
it:

```cpp
WebhookSignatureConfig cfg;
cfg.maxClockSkew = std::chrono::minutes(5);   // opt in to replay protection
client.setWebhookSignatureVerifier(makeDefaultWebhookSignatureVerifier(cfg));
```

| Field | Default | Meaning |
|---|---|---|
| `keyServerBaseUrl` | `https://key.smartthings.com` | Where public keys are fetched (`baseUrl + keyId`); overridable for tests |
| `maxClockSkew` | `0` (off) | If non-zero, reject requests whose signed `Date` is farther than this from now. Off by default — matching the reference SmartThings SDK — because the signature already authenticates the `Date`, and an always-on window would falsely reject legitimate events on clock drift or delivery delay |
| `keyCacheTtl` | 1 hour | How long a fetched public key is cached. Kept short because keys rotate |

## Platform support

> [!IMPORTANT]
> The default verifier performs the RSA verification with **native Windows CNG**
> (BCrypt/CryptoAPI) — this toolchain deliberately carries no OpenSSL. On
> Linux/macOS the default verifier's `verify()` fails with a clear message:
> inject your own implementation there (an OpenSSL-based sketch is in
> [Extending the library](extending.md#iwebhooksignatureverifier)).

The parsing and canonicalization half (`http_signature::parseSignatureParams`,
`buildSigningString`, `parseHttpDate`) is pure, platform-independent, and
unit-tested offline — a custom verifier only needs to supply the crypto.

## Replacing or disabling verification

```cpp
// Custom verifier (your crypto backend, your policy):
client.setWebhookSignatureVerifier(std::make_unique<MyVerifier>());

// Disable entirely - the webhook then trusts anything that POSTs to it:
client.setWebhookSignatureVerifier(nullptr);
```

> [!WARNING]
> Only disable verification for local experiments. A webhook without signature
> verification will happily process forged events from anyone who finds the
> URL.

Note the string overload `handleWebhook(const std::string& body)` cannot verify
(it has no headers to check) — it exists for tests and manual dispatch. Real
requests flow through `handleWebhook(const WebhookRequest&)`, which verifies
whenever a verifier is set.

## Testing with a key resolver

The verifier factory takes an optional `PublicKeyResolver` — a function from
`keyId` to PEM public key — so tests can verify against a local fixture key
instead of the live key server:

```cpp
auto verifier = makeDefaultWebhookSignatureVerifier({},
    [](const std::string& keyId) -> std::optional<std::string> {
        if (keyId == "/test/key") return TEST_PUBLIC_KEY_PEM;
        return std::nullopt;
    });
client.setWebhookSignatureVerifier(std::move(verifier));
```

---

<div align="center">

[← Live updates](live-updates.md) · **Next:** [Extending the library →](extending.md)

</div>
