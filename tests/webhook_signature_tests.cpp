#include <catch2/catch_test_macros.hpp>

#include "smartthings4cpp/webhook_signature.h"
#include "smartthings4cpp/webhook.h"
#include "smartthings4cpp/base64.h"

#include <chrono>
#include <optional>
#include <string>

using namespace smartthings4cpp;

// A real RSA-2048 test vector (public key + a genuine rsa-sha256 signature over
// the exact canonical signing string this library builds) generated once with
// .NET's System.Security.Cryptography - see tools/scratch. It lets the native
// Windows-CNG verification be exercised end-to-end offline, with no network and
// no private key in the tree.
namespace {

	const char* kPublicKeyPem =
		"-----BEGIN PUBLIC KEY-----\n"
		"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxA1BPVR2qDX2IW7zZ98A\n"
		"Az2BwrqK9hH2flPPTwI5wq/4K3O/qsCZwzvLYXMdGsm2N89melNZ+HW/vt0xXFt9\n"
		"lCQHOfdZ7co4bjr2k6yHc8b8A7n5uAFIbXHQAUeKDwX2HUXXTwHyQigeYNB/uqmq\n"
		"RLvdgW8ZpS6c6D2xl2hkdZMrWPM3jn5Gn07QlT6rDy51dxEPN3vd5mbS3O2Szwrs\n"
		"D+0gmXD4EcB4U77V3diANmp8yMBT+0QB+e+U1Xd6cAPCDxR4nAeia+0po4lhGXtE\n"
		"J8fePNoYqmqMZ1EotWAfjhl09SKf4hq6BVnVbwbKtKs7mZHqFjNxRkcnQYpK1uCd\n"
		"5QIDAQAB\n"
		"-----END PUBLIC KEY-----\n";

	const char* kBody = R"({"messageType":"PING","pingData":{"challenge":"abc"}})";
	const char* kDate = "Tue, 05 Jul 2026 12:00:00 GMT";
	const char* kDigestHeader = "SHA-256=Kglin3mruEI43G8DFioCkQe8Knb4ElHCkbzl8orEXEI=";
	const char* kSignatureB64 =
		"XH7g/b7bmzFMRhxCexmTgWDN7oWpX0tDhGb/LT28ETaQgw602Zqj+VymflJiQnoZGGvyHULwbi+"
		"XwZn6vc7OpU/EVxsEFM5UXwIoYhNiTPULlAgGefIzt5pPJwcoV+UCyHE/fTQnui0ScY0S4FSS/kQ"
		"NIrqaUgqbnYH7JsnkeH1uvzVfp8bLMLi/RFpWOusgI7Pe+PGuPoJ7II04Vnp1Mp4r1la6mFb5mc4"
		"pqBP9qzyUcc3AGxxcpA6BJmkn2xxMS2KaoTCfqj5/gwryEd9VjgA8r7Dx7luvkdol0GgK/ZMFwug"
		"iyf3rzJp5wUZ+Yk9OBxKtJBDEvp58zA+XE63X+A==";

	std::string authHeaderWith(const std::string& signatureB64) {
		return std::string("Signature keyId=\"/test/key1\","
			"headers=\"(request-target) digest date\",algorithm=\"rsa-sha256\",")
			+ "signature=\"" + signatureB64 + "\"";
	}

	// The signed vector as a WebhookRequest (headers lowercased, as the server
	// stores them).
	WebhookRequest signedRequest() {
		WebhookRequest req;
		req.method = "POST";
		req.path = "/webhook";
		req.body = kBody;
		req.headers["digest"] = kDigestHeader;
		req.headers["date"] = kDate;
		req.headers["authorization"] = authHeaderWith(kSignatureB64);
		return req;
	}

	PublicKeyResolver fixtureResolver() {
		return [](const std::string& keyId) -> std::optional<std::string> {
			if (keyId == "/test/key1") {
				return std::string(kPublicKeyPem);
			}
			return std::nullopt;
		};
	}

	// The fixture Date is fixed, so disable freshness for signature-focused tests.
	WebhookSignatureConfig noSkewConfig() {
		WebhookSignatureConfig config;
		config.maxClockSkew = std::chrono::seconds(0);
		return config;
	}

} // namespace

// --- pure parsing / canonicalization (no crypto) -----------------------------

TEST_CASE("parseSignatureParams reads a SmartThings Authorization header", "[signature]") {
	auto params = http_signature::parseSignatureParams(authHeaderWith("QUJD"));
	REQUIRE(params.has_value());
	REQUIRE(params->keyId == "/test/key1");
	REQUIRE(params->algorithm == "rsa-sha256");
	REQUIRE(params->signatureBase64 == "QUJD");
	REQUIRE(params->headers == std::vector<std::string>{ "(request-target)", "digest", "date" });
}

TEST_CASE("parseSignatureParams tolerates a bare (scheme-less) header", "[signature]") {
	auto params = http_signature::parseSignatureParams(
		R"(keyId="/k", signature="QQ==", algorithm="RSA-SHA256")");
	REQUIRE(params.has_value());
	REQUIRE(params->keyId == "/k");
	REQUIRE(params->algorithm == "rsa-sha256"); // lowercased
	REQUIRE(params->headers == std::vector<std::string>{ "date" }); // draft default
}

TEST_CASE("parseSignatureParams rejects a header missing keyId or signature", "[signature]") {
	REQUIRE_FALSE(http_signature::parseSignatureParams(
		R"(Signature headers="date",algorithm="rsa-sha256")").has_value());
	REQUIRE_FALSE(http_signature::parseSignatureParams(
		R"(Signature keyId="/k",algorithm="rsa-sha256")").has_value());
	REQUIRE_FALSE(http_signature::parseSignatureParams("not a signature at all").has_value());
}

TEST_CASE("buildSigningString assembles the canonical draft-cavage string", "[signature]") {
	auto req = signedRequest();
	auto signing = http_signature::buildSigningString(
		{ "(request-target)", "digest", "date" }, req);
	REQUIRE(signing.has_value());
	REQUIRE(*signing ==
		std::string("(request-target): post /webhook\n")
		+ "digest: " + kDigestHeader + "\n"
		+ "date: " + kDate);
}

TEST_CASE("buildSigningString fails when a signed header is absent", "[signature]") {
	WebhookRequest req;
	req.method = "POST";
	req.path = "/webhook";
	// No "date" header present, but it is listed as signed.
	auto signing = http_signature::buildSigningString({ "(request-target)", "date" }, req);
	REQUIRE_FALSE(signing.has_value());
}

TEST_CASE("parseHttpDate parses an IMF-fixdate and rejects garbage", "[signature]") {
	auto parsed = http_signature::parseHttpDate("Sun, 06 Nov 1994 08:49:37 GMT");
	REQUIRE(parsed.has_value());
	// 1994-11-06T08:49:37Z == 784111777 seconds since the epoch.
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
		parsed->time_since_epoch()).count();
	REQUIRE(seconds == 784111777);

	REQUIRE_FALSE(http_signature::parseHttpDate("not a date").has_value());
	REQUIRE_FALSE(http_signature::parseHttpDate("Sun, 06 Zzz 1994 08:49:37 GMT").has_value());
}

TEST_CASE("base64::decode round-trips and rejects invalid input", "[signature]") {
	REQUIRE(base64::decode("aGVsbG8=").value_or("") == "hello");
	REQUIRE(base64::decode(base64::encode("smartthings")).value_or("") == "smartthings");
	// Whitespace (e.g. wrapped PEM lines) is ignored.
	REQUIRE(base64::decode("aGVs\nbG8=").value_or("") == "hello");
	// Characters outside the alphabet are invalid.
	REQUIRE_FALSE(base64::decode("not*base64").has_value());
}

// --- end-to-end verification (native Windows CNG crypto) ---------------------

#ifdef _WIN32

TEST_CASE("verify() accepts a genuinely-signed request", "[signature]") {
	auto verifier = makeDefaultWebhookSignatureVerifier(noSkewConfig(), fixtureResolver());
	auto result = verifier->verify(signedRequest());
	INFO(result.error_message);
	REQUIRE(result.isSuccess());
}

TEST_CASE("verify() rejects a tampered body (digest mismatch)", "[signature]") {
	auto verifier = makeDefaultWebhookSignatureVerifier(noSkewConfig(), fixtureResolver());
	auto req = signedRequest();
	req.body += " "; // one extra byte: SHA-256 no longer matches the Digest header
	REQUIRE_FALSE(verifier->verify(req).isSuccess());
}

TEST_CASE("verify() rejects a tampered signature", "[signature]") {
	auto verifier = makeDefaultWebhookSignatureVerifier(noSkewConfig(), fixtureResolver());
	auto req = signedRequest();
	std::string flipped = kSignatureB64;
	flipped[0] = (flipped[0] == 'X') ? 'Y' : 'X'; // still valid base64, wrong bytes
	req.headers["authorization"] = authHeaderWith(flipped);
	REQUIRE_FALSE(verifier->verify(req).isSuccess());
}

TEST_CASE("verify() rejects a request with no signature header", "[signature]") {
	auto verifier = makeDefaultWebhookSignatureVerifier(noSkewConfig(), fixtureResolver());
	auto req = signedRequest();
	req.headers.erase("authorization");
	auto result = verifier->verify(req);
	REQUIRE_FALSE(result.isSuccess());
	REQUIRE(result.error == ErrorCode::AuthenticationFailed);
}

TEST_CASE("verify() rejects an unsupported algorithm", "[signature]") {
	auto verifier = makeDefaultWebhookSignatureVerifier(noSkewConfig(), fixtureResolver());
	auto req = signedRequest();
	req.headers["authorization"] =
		R"(Signature keyId="/test/key1",headers="(request-target) digest date",)"
		R"(algorithm="hmac-sha256",signature="QUJD")";
	REQUIRE_FALSE(verifier->verify(req).isSuccess());
}

TEST_CASE("verify() fails when the public key cannot be resolved", "[signature]") {
	// A resolver that never returns a key (e.g. key-server fetch failed).
	auto verifier = makeDefaultWebhookSignatureVerifier(noSkewConfig(),
		[](const std::string&) -> std::optional<std::string> { return std::nullopt; });
	REQUIRE_FALSE(verifier->verify(signedRequest()).isSuccess());
}

TEST_CASE("verify() rejects a stale Date when freshness is enabled", "[signature]") {
	// Freshness is opt-in: enable it, then a decades-old Date is outside the
	// window. The Digest still matches the body, so the request reaches the
	// Date check.
	WebhookSignatureConfig config;
	config.maxClockSkew = std::chrono::minutes(5);
	auto verifier = makeDefaultWebhookSignatureVerifier(config, fixtureResolver());
	auto req = signedRequest();
	req.headers["date"] = "Tue, 05 Jul 1994 12:00:00 GMT";
	REQUIRE_FALSE(verifier->verify(req).isSuccess());
}

TEST_CASE("verify() accepts a real captured SmartThings EVENT request", "[signature]") {
	// A genuine request captured from SmartThings: its public X.509 certificate
	// (fetched once from key.smartthings.com) plus the real rsa-sha256 signature.
	// Proves the production path end-to-end offline - the CERTIFICATE key form,
	// the "SHA256=" (no hyphen) Digest token SmartThings actually sends, and the
	// exact canonical signing string. Freshness is off by default, so the
	// captured (now-old) Date is accepted.
	const char* body =
		R"BODY({"messageType":"EVENT","eventData":{"installedApp":{"installedAppId":"7852314e-0d64-4adc-be03-752458eba6e1","locationId":"3c63b228-a999-42b7-b635-5e04cff0e1b6"},"events":[{"eventTime":"2026-07-08T18:36:50Z","eventType":"DEVICE_EVENT","deviceEvent":{"eventId":"fba06a38-7afb-11f1-b5e6-d369b2c5435d","locationId":"3c63b228-a999-42b7-b635-5e04cff0e1b6","ownerId":"3c63b228-a999-42b7-b635-5e04cff0e1b6","ownerType":"LOCATION","deviceId":"273c2d43-12d3-4e32-90ad-cbcbfb33f102","componentId":"main","capability":"synthetic.lightingEffectCircadian","attribute":"circadian","value":{"timeZoneId":"Europe/Paris","state":"Run"},"valueType":"string","unit":"","stateChange":true,"data":{},"subscriptionName":"6de9b613-b044-4bb0-938e-75581809a0ef","sensitive":false}}]}})BODY";

	const char* certPem =
		"-----BEGIN CERTIFICATE-----\n"
		"MIIDvDCCAqSgAwIBAgIUG2TbLWLh7TTBwx8ntDYXcmvhUTQwDQYJKoZIhvcNAQEL\n"
		"BQAwUTEYMAoGA1UEBhMDS09SMAoGA1UEBhMDVVNBMRQwEgYDVQQKEwtTbWFydFRo\n"
		"aW5nczEfMB0GA1UEAxMWU21hcnRUaGluZ3MgUGFkbG9jayBDQTAeFw0yNjA3MDMx\n"
		"OTQxMzBaFw0yNjA4MDIxOTQxNTlaMEoxFDASBgNVBAoTC1NtYXJ0VGhpbmdzMRQw\n"
		"EgYDVQQLEwtFbmdpbmVlcmluZzEcMBoGA1UEAxMTa2V5LnNtYXJ0dGhpbmdzLmNv\n"
		"bTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMYURQJouhdRAA0dqaOd\n"
		"4p8rYvQpq+QyQo6umvbsJhgKdWcgLJGeXsEDhtJd9TNyjNVvu6n3+GHPdflOIPWT\n"
		"2P6+CUebnbPBG0oZFA+/Q1BVwJpzknjsJR1IwTnTw6dt8hJ1kp3c1/76eJYcmtjg\n"
		"7oqjWvCdp/HIJ6/i+hWgszYHflJrEq1ophqDa4fEIgNYdV67XIhclGHpX49iMYEP\n"
		"eccqCbZ8fP9R83Jhxzh6aJ9pmCeMZ5TnmeqTP725W2k4ODBqyOZ5eVodG01zgRyz\n"
		"HmEDE7IW6+YAviO77soSNR2oqUiYVGhET6tTGFG40nLYLlDNRkrIVbvpFwn1dPNw\n"
		"4mMCAwEAAaOBkjCBjzAOBgNVHQ8BAf8EBAMCB4AwHQYDVR0lBBYwFAYIKwYBBQUH\n"
		"AwEGCCsGAQUFBwMCMB0GA1UdDgQWBBScHF1sHRKEGVJHe7ToKKMIc7Jy+zAfBgNV\n"
		"HSMEGDAWgBQltyyqmlsKclo60XEDHSwMHegFTTAeBgNVHREEFzAVghNrZXkuc21h\n"
		"cnR0aGluZ3MuY29tMA0GCSqGSIb3DQEBCwUAA4IBAQBpsN24Q5XaulGtU0o7UNTV\n"
		"OVUlAwLqvL/vL8I/nVpxzw3aTpXeKuBV5xWjBEcE/4xSHr4f0L9g0w6/kAAW09E5\n"
		"EOkOu3S2c/cZvfTgPyZjad5jmMFE2py1l45dMogWlkQ32362MZ1v3D0UqBzUP2Bq\n"
		"skjqsaSr5/u1YaFLLLRMQjMN9vy5glL668KYzNonVY4idw8/CJnNVyHhb6DHf/B2\n"
		"mUm3W9TsPuL/WmVu6LX+TfcQxGhDs8oYChBlg5YeIGjKY8wjRuQiHGqGtUszrZ23\n"
		"+f0b0BbStdFBXpYC9fHwbZ3f+4+apgkmering7EH1vVRCQtYlrpryJtlxLkBe+/Z\n"
		"-----END CERTIFICATE-----\n";

	const std::string keyId =
		"/pl/euwest1/1b-64-db-2d-62-e1-ed-34-c1-c3-1f-27-b4-36-17-72-6b-e1-51-34";

	WebhookRequest req;
	req.method = "POST";
	req.path = "/";
	req.body = body;
	req.headers["digest"] = "SHA256=1czpIDSxFlGDuP+7Leu5g9zWTIBiLzXvygRWzqxLd2s=";
	req.headers["date"] = "Wed, 08 Jul 2026 18:36:50 UTC";
	req.headers["authorization"] =
		R"(Signature keyId="/pl/euwest1/1b-64-db-2d-62-e1-ed-34-c1-c3-1f-27-b4-36-17-72-6b-e1-51-34",)"
		R"(signature="A+cvYhx3YhALYvq7dKfjHjObj9m4RDnRUR9jO7Ur2ZCBdAui6CXUGcFHjqctwEJb+l/6bBoj7DlsI6Ho/8Ef7KI0Zez29lLoumcPK2VX774vre17dsZ+Glf4Hz4V4Sbjg/WKUI/kiaZy+plRUcw9tfPA3JiWhMpSqMwTucE8vaqPmTsJPdsXtfzgLqelefAHS9BL9ZCsXp6kgNN/wW+s9WBoN/SY6lzInrGWHLByc/JCXVzk7Mgy34diS6X4W0tJ+mrZUBX3S25o0gdFXuQnRZ2P/pwUYk7vQ9jB0OmJWEMnqW8Sno2AKt0MMUwH8HbZHMrWPqsJkAM/4p6fK/npqw==",)"
		R"(headers="(request-target) digest date",algorithm="rsa-sha256")";

	auto verifier = makeDefaultWebhookSignatureVerifier(WebhookSignatureConfig{},
		[&](const std::string& id) -> std::optional<std::string> {
			if (id == keyId) {
				return std::string(certPem);
			}
			return std::nullopt;
		});

	auto result = verifier->verify(req);
	INFO(result.error_message);
	REQUIRE(result.isSuccess());
}

#endif // _WIN32
