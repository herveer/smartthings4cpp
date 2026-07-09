#include "smartthings4cpp/webhook_signature.h"
#include "smartthings4cpp/base64.h"
#include "smartthings4cpp/http_client.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <ctime>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

// Native RSA verification via Windows CNG - there is no OpenSSL in this
// toolchain (curl here is built against Schannel). Same native-on-Windows
// approach storage.cpp takes with the Credential Manager.
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <wincrypt.h>
#include <bcrypt.h>
#endif

/**
 * @file webhook_signature.cpp
 * @brief HTTP-Signature parsing/canonicalization + the default (CNG-backed)
 *        webhook signature verifier. See webhook_signature.h.
 */

namespace smartthings4cpp {

	namespace {

		std::string toLower(std::string s) {
			std::transform(s.begin(), s.end(), s.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return s;
		}

		std::string trim(const std::string& s) {
			const auto first = s.find_first_not_of(" \t\r\n");
			if (first == std::string::npos) {
				return "";
			}
			const auto last = s.find_last_not_of(" \t\r\n");
			return s.substr(first, last - first + 1);
		}

		// Split on ASCII whitespace, lowercasing each token (used for the space
		// separated `headers` list of a Signature header).
		std::vector<std::string> splitLowerWords(const std::string& s) {
			std::vector<std::string> words;
			std::istringstream iss(s);
			std::string word;
			while (iss >> word) {
				words.push_back(toLower(word));
			}
			return words;
		}

	} // namespace

	// --- pure HTTP-Signature helpers ---------------------------------------

	namespace http_signature {

		std::optional<SignatureParams> parseSignatureParams(const std::string& headerValue) {
			std::string s = headerValue;
			size_t i = 0;
			while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) {
				++i;
			}

			// Optional "Signature" scheme token (the Authorization header carries
			// it; a bare Signature header does not).
			static const std::string scheme = "signature";
			if (s.size() - i >= scheme.size()) {
				bool match = true;
				for (size_t k = 0; k < scheme.size(); ++k) {
					if (std::tolower(static_cast<unsigned char>(s[i + k])) != scheme[k]) {
						match = false;
						break;
					}
				}
				if (match && i + scheme.size() < s.size()
					&& std::isspace(static_cast<unsigned char>(s[i + scheme.size()]))) {
					i += scheme.size();
					while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) {
						++i;
					}
				}
			}

			SignatureParams params;
			bool haveKeyId = false;
			bool haveSignature = false;

			while (i < s.size()) {
				while (i < s.size()
					&& (std::isspace(static_cast<unsigned char>(s[i])) || s[i] == ',')) {
					++i;
				}
				if (i >= s.size()) {
					break;
				}

				const size_t keyStart = i;
				while (i < s.size() && s[i] != '=' && s[i] != ',') {
					++i;
				}
				if (i >= s.size() || s[i] != '=') {
					continue; // no '=': not a key=value pair, skip to the next comma
				}
				const std::string key = toLower(trim(s.substr(keyStart, i - keyStart)));
				++i; // consume '='

				std::string value;
				if (i < s.size() && s[i] == '"') {
					++i;
					const size_t valStart = i;
					while (i < s.size() && s[i] != '"') {
						++i;
					}
					value = s.substr(valStart, i - valStart);
					if (i < s.size()) {
						++i; // consume closing quote
					}
				}
				else {
					const size_t valStart = i;
					while (i < s.size() && s[i] != ',') {
						++i;
					}
					value = trim(s.substr(valStart, i - valStart));
				}

				if (key == "keyid") {
					params.keyId = value;
					haveKeyId = true;
				}
				else if (key == "algorithm") {
					params.algorithm = toLower(value);
				}
				else if (key == "signature") {
					params.signatureBase64 = value;
					haveSignature = true;
				}
				else if (key == "headers") {
					params.headers = splitLowerWords(value);
				}
				// created/expires and any unknown params are ignored.
			}

			if (!haveKeyId || !haveSignature || params.signatureBase64.empty()) {
				return std::nullopt;
			}
			if (params.headers.empty()) {
				params.headers = { "date" }; // draft-cavage default when omitted
			}
			return params;
		}

		std::optional<std::string> buildSigningString(
			const std::vector<std::string>& signedHeaders, const WebhookRequest& request) {
			std::string out;
			for (size_t idx = 0; idx < signedHeaders.size(); ++idx) {
				const std::string& name = signedHeaders[idx]; // already lowercased
				std::string line;
				if (name == "(request-target)") {
					line = name + ": " + toLower(request.method) + " " + request.path;
				}
				else if (name == "(created)" || name == "(expires)") {
					// SmartThings signs concrete headers, not these synthetic
					// params; without their signature-parameter values the string
					// cannot be reconstructed.
					return std::nullopt;
				}
				else {
					auto value = request.header(name);
					if (!value) {
						return std::nullopt; // a signed header is missing: unverifiable
					}
					line = name + ": " + trim(*value);
				}
				if (idx > 0) {
					out += "\n";
				}
				out += line;
			}
			return out;
		}

		std::optional<std::chrono::system_clock::time_point> parseHttpDate(const std::string& httpDate) {
			std::istringstream iss(httpDate);
			std::vector<std::string> tokens;
			std::string token;
			while (iss >> token) {
				tokens.push_back(token);
			}
			// IMF-fixdate: "Sun, 06 Nov 1994 08:49:37 GMT" -> 6 tokens. Tolerate a
			// missing weekday by detecting whether the first token is numeric.
			size_t offset = 0;
			if (!tokens.empty() && !tokens[0].empty()
				&& !std::isdigit(static_cast<unsigned char>(tokens[0][0]))) {
				offset = 1; // skip the weekday token ("Sun,")
			}
			if (tokens.size() < offset + 4) {
				return std::nullopt;
			}

			static const std::array<const char*, 12> months = {
				"jan", "feb", "mar", "apr", "may", "jun",
				"jul", "aug", "sep", "oct", "nov", "dec" };

			auto parseInt = [](const std::string& str, int& out) -> bool {
				if (str.empty()) {
					return false;
				}
				try {
					size_t consumed = 0;
					out = std::stoi(str, &consumed);
					return consumed == str.size();
				}
				catch (const std::exception&) {
					return false;
				}
			};

			int day = 0;
			int year = 0;
			if (!parseInt(tokens[offset], day) || !parseInt(tokens[offset + 2], year)) {
				return std::nullopt;
			}

			int month = -1;
			const std::string monthName = toLower(tokens[offset + 1]).substr(0, 3);
			for (int m = 0; m < 12; ++m) {
				if (monthName == months[static_cast<size_t>(m)]) {
					month = m;
					break;
				}
			}
			if (month < 0) {
				return std::nullopt;
			}

			// "HH:MM:SS"
			const std::string& timePart = tokens[offset + 3];
			const auto firstColon = timePart.find(':');
			const auto secondColon = timePart.find(':', firstColon + 1);
			if (firstColon == std::string::npos || secondColon == std::string::npos) {
				return std::nullopt;
			}
			int hour = 0;
			int minute = 0;
			int second = 0;
			if (!parseInt(timePart.substr(0, firstColon), hour)
				|| !parseInt(timePart.substr(firstColon + 1, secondColon - firstColon - 1), minute)
				|| !parseInt(timePart.substr(secondColon + 1), second)) {
				return std::nullopt;
			}

			std::tm tm{};
			tm.tm_mday = day;
			tm.tm_mon = month;
			tm.tm_year = year - 1900;
			tm.tm_hour = hour;
			tm.tm_min = minute;
			tm.tm_sec = second;

#ifdef _WIN32
			const std::time_t tt = _mkgmtime(&tm);
#else
			const std::time_t tt = timegm(&tm);
#endif
			if (tt == static_cast<std::time_t>(-1)) {
				return std::nullopt;
			}
			return std::chrono::system_clock::from_time_t(tt);
		}

	} // namespace http_signature

	// --- native crypto (Windows CNG) ---------------------------------------

	namespace {

#ifdef _WIN32
		bool cryptoAvailable() { return true; }

		// SHA-256 of `data`, returned as 32 raw bytes (std::nullopt on failure).
		std::optional<std::string> sha256Raw(const std::string& data) {
			BCRYPT_ALG_HANDLE alg = nullptr;
			if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0) {
				return std::nullopt;
			}

			DWORD hashLen = 0;
			DWORD produced = 0;
			std::optional<std::string> result;
			if (BCryptGetProperty(alg, BCRYPT_HASH_LENGTH,
				reinterpret_cast<PUCHAR>(&hashLen), sizeof(hashLen), &produced, 0) == 0
				&& hashLen > 0) {
				BCRYPT_HASH_HANDLE hash = nullptr;
				if (BCryptCreateHash(alg, &hash, nullptr, 0, nullptr, 0, 0) == 0) {
					if (BCryptHashData(hash,
						reinterpret_cast<PUCHAR>(const_cast<char*>(data.data())),
						static_cast<ULONG>(data.size()), 0) == 0) {
						std::string out(hashLen, '\0');
						if (BCryptFinishHash(hash,
							reinterpret_cast<PUCHAR>(out.data()), hashLen, 0) == 0) {
							result = std::move(out);
						}
					}
					BCryptDestroyHash(hash);
				}
			}
			BCryptCloseAlgorithmProvider(alg, 0);
			return result;
		}

		// Import a PEM public key (SubjectPublicKeyInfo "BEGIN PUBLIC KEY", or an
		// X.509 "BEGIN CERTIFICATE" whose public key we extract) as a CNG key.
		BCRYPT_KEY_HANDLE importPublicKey(const std::string& pem) {
			DWORD derLen = 0;
			if (!CryptStringToBinaryA(pem.c_str(), static_cast<DWORD>(pem.size()),
				CRYPT_STRING_BASE64HEADER, nullptr, &derLen, nullptr, nullptr) || derLen == 0) {
				return nullptr;
			}
			std::vector<BYTE> der(derLen);
			if (!CryptStringToBinaryA(pem.c_str(), static_cast<DWORD>(pem.size()),
				CRYPT_STRING_BASE64HEADER, der.data(), &derLen, nullptr, nullptr)) {
				return nullptr;
			}

			BCRYPT_KEY_HANDLE key = nullptr;
			if (pem.find("CERTIFICATE") != std::string::npos) {
				PCCERT_CONTEXT cert = CertCreateCertificateContext(
					X509_ASN_ENCODING, der.data(), derLen);
				if (cert) {
					CryptImportPublicKeyInfoEx2(X509_ASN_ENCODING,
						&cert->pCertInfo->SubjectPublicKeyInfo, 0, nullptr, &key);
					CertFreeCertificateContext(cert);
				}
			}
			else {
				CERT_PUBLIC_KEY_INFO* info = nullptr;
				DWORD infoLen = 0;
				if (CryptDecodeObjectEx(X509_ASN_ENCODING, X509_PUBLIC_KEY_INFO,
					der.data(), derLen, CRYPT_DECODE_ALLOC_FLAG, nullptr,
					&info, &infoLen)) {
					CryptImportPublicKeyInfoEx2(X509_ASN_ENCODING, info, 0, nullptr, &key);
					LocalFree(info);
				}
			}
			return key;
		}

		bool rsaSha256Verify(const std::string& pem, const std::string& message,
			const std::string& signature) {
			auto hash = sha256Raw(message);
			if (!hash) {
				return false;
			}
			BCRYPT_KEY_HANDLE key = importPublicKey(pem);
			if (!key) {
				return false;
			}
			BCRYPT_PKCS1_PADDING_INFO padding{ BCRYPT_SHA256_ALGORITHM };
			const NTSTATUS status = BCryptVerifySignature(key, &padding,
				reinterpret_cast<PUCHAR>(const_cast<char*>(hash->data())),
				static_cast<ULONG>(hash->size()),
				reinterpret_cast<PUCHAR>(const_cast<char*>(signature.data())),
				static_cast<ULONG>(signature.size()),
				BCRYPT_PAD_PKCS1);
			BCryptDestroyKey(key);
			return status == 0; // STATUS_SUCCESS
		}
#else
		bool cryptoAvailable() { return false; }
		std::optional<std::string> sha256Raw(const std::string&) { return std::nullopt; }
		bool rsaSha256Verify(const std::string&, const std::string&, const std::string&) {
			return false;
		}
#endif

		// Recompute the body's SHA-256 and compare it to the request's Digest
		// header ("SHA-256=<base64>", possibly a comma-separated list).
		bool verifyDigest(const std::string& body, const std::string& digestHeader) {
			auto hash = sha256Raw(body);
			if (!hash) {
				return false;
			}
			const std::string expected = base64::encode(*hash);

			std::stringstream stream(digestHeader);
			std::string part;
			while (std::getline(stream, part, ',')) {
				const std::string trimmed = trim(part);
				const auto eq = trimmed.find('=');
				if (eq == std::string::npos) {
					continue;
				}
				// SmartThings sends "SHA256=..."; RFC 3230 also allows "SHA-256=...".
				const std::string algorithm = toLower(trim(trimmed.substr(0, eq)));
				if (algorithm == "sha256" || algorithm == "sha-256") {
					return trim(trimmed.substr(eq + 1)) == expected;
				}
			}
			return false; // no SHA-256 digest offered: cannot confirm the body
		}

		Result<void> fail(const std::string& why) {
			return Result<void>(ErrorCode::AuthenticationFailed, "webhook signature: " + why);
		}

		/**
		 * The default verifier: HTTP-Signature (rsa-sha256) verification against
		 * SmartThings' public key, fetched from the key server and cached briefly.
		 */
		class DefaultWebhookSignatureVerifier : public IWebhookSignatureVerifier {
		public:
			DefaultWebhookSignatureVerifier(WebhookSignatureConfig config, PublicKeyResolver resolver)
				: _config(std::move(config)), _resolver(std::move(resolver)) {
				while (!_config.keyServerBaseUrl.empty() && _config.keyServerBaseUrl.back() == '/') {
					_config.keyServerBaseUrl.pop_back();
				}
			}

			Result<void> verify(const WebhookRequest& request) override {
				if (!cryptoAvailable()) {
					return fail("no crypto backend on this platform - inject an "
						"IWebhookSignatureVerifier via Client::setWebhookSignatureVerifier()");
				}

				auto authHeader = request.header("authorization");
				if (!authHeader) {
					authHeader = request.header("signature");
				}
				if (!authHeader) {
					return fail("no Authorization or Signature header");
				}

				auto params = http_signature::parseSignatureParams(*authHeader);
				if (!params) {
					return fail("malformed signature header");
				}
				if (!params->algorithm.empty() && params->algorithm != "rsa-sha256") {
					return fail("unsupported algorithm '" + params->algorithm + "'");
				}

				// Body integrity: the signature covers the Digest header's value,
				// so the body itself is only trustworthy once we confirm its hash
				// equals that Digest.
				const bool digestSigned = std::find(params->headers.begin(),
					params->headers.end(), "digest") != params->headers.end();
				auto digestHeader = request.header("digest");
				if (digestSigned || digestHeader) {
					if (!digestHeader) {
						return fail("Digest header missing but signed");
					}
					if (!verifyDigest(request.body, *digestHeader)) {
						return fail("body does not match the Digest header");
					}
				}

				// Replay protection via the signed Date (best-effort, optional).
				if (_config.maxClockSkew.count() > 0) {
					if (auto dateHeader = request.header("date")) {
						auto when = http_signature::parseHttpDate(*dateHeader);
						if (!when) {
							return fail("unparseable Date header");
						}
						const auto now = std::chrono::system_clock::now();
						const auto diff = std::chrono::duration_cast<std::chrono::seconds>(
							now > *when ? now - *when : *when - now);
						if (diff > _config.maxClockSkew) {
							return fail("stale Date (outside the allowed clock skew)");
						}
					}
				}

				auto signingString = http_signature::buildSigningString(params->headers, request);
				if (!signingString) {
					return fail("a signed header is missing from the request");
				}

				auto signatureBytes = base64::decode(params->signatureBase64);
				if (!signatureBytes) {
					return fail("signature is not valid base64");
				}

				auto pem = resolveKey(params->keyId);
				if (!pem) {
					return fail("could not retrieve the public key for keyId '" + params->keyId + "'");
				}

				if (!rsaSha256Verify(*pem, *signingString, *signatureBytes)) {
					return fail("signature does not match");
				}
				return Result<void>();
			}

		private:
			std::optional<std::string> resolveKey(const std::string& keyId) {
				const auto now = std::chrono::steady_clock::now();
				{
					std::lock_guard<std::mutex> lock(_cacheMutex);
					auto it = _cache.find(keyId);
					if (it != _cache.end() && it->second.second > now) {
						return it->second.first;
					}
				}

				std::optional<std::string> pem = _resolver
					? _resolver(keyId) : httpFetchKey(keyId);

				if (pem && !pem->empty()) {
					std::lock_guard<std::mutex> lock(_cacheMutex);
					_cache[keyId] = { *pem, now + _config.keyCacheTtl };
				}
				return pem;
			}

			std::optional<std::string> httpFetchKey(const std::string& keyId) {
				if (keyId.empty()) {
					return std::nullopt;
				}
				std::string path = keyId.front() == '/' ? keyId : "/" + keyId;
				HttpClient http;
				http.setTimeout(std::chrono::milliseconds(10000));
				auto response = http.get(_config.keyServerBaseUrl + path);
				if (response.isSuccess() && !response.body.empty()) {
					return response.body;
				}
				return std::nullopt;
			}

			WebhookSignatureConfig _config;
			PublicKeyResolver _resolver;
			std::mutex _cacheMutex;
			// keyId -> (PEM, expiry)
			std::unordered_map<std::string,
				std::pair<std::string, std::chrono::steady_clock::time_point>> _cache;
		};

	} // namespace

	std::unique_ptr<IWebhookSignatureVerifier> makeDefaultWebhookSignatureVerifier(
		WebhookSignatureConfig config, PublicKeyResolver keyResolver) {
		return std::make_unique<DefaultWebhookSignatureVerifier>(
			std::move(config), std::move(keyResolver));
	}

} // namespace smartthings4cpp
