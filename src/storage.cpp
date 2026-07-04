#include "smartthings4cpp/storage.h"
#include "smartthings4cpp/json_utils.h"

#include <nlohmann/json.hpp>
#include <fstream>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <wincred.h>
#endif

namespace smartthings4cpp {

	// --- FileStorageProvider -------------------------------------------------

	FileStorageProvider::FileStorageProvider(std::filesystem::path filePath)
		: _filePath(std::move(filePath)) {
	}

	std::map<std::string, std::string> FileStorageProvider::load() const {
		std::map<std::string, std::string> values;
		try {
			std::ifstream file(_filePath);
			if (!file.is_open()) {
				return values;
			}
			std::string content((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());
			auto json = json_utils::parse(content);
			if (json.is_object()) {
				for (auto it = json.begin(); it != json.end(); ++it) {
					if (it.value().is_string()) {
						values[it.key()] = it.value().get<std::string>();
					}
				}
			}
		}
		catch (const std::exception&) {
			// Missing/corrupt file behaves as empty.
		}
		return values;
	}

	bool FileStorageProvider::save(const std::map<std::string, std::string>& values) const {
		try {
			if (_filePath.has_parent_path()) {
				std::filesystem::create_directories(_filePath.parent_path());
			}
			nlohmann::json json = nlohmann::json::object();
			for (const auto& [key, value] : values) {
				json[key] = value;
			}
			std::ofstream file(_filePath, std::ios::trunc);
			if (!file.is_open()) {
				return false;
			}
			file << json.dump(2);
			return file.good();
		}
		catch (const std::exception&) {
			return false;
		}
	}

	std::optional<std::string> FileStorageProvider::getValue(const std::string& key) {
		std::lock_guard<std::mutex> lock(_mutex);
		auto values = load();
		auto it = values.find(key);
		if (it == values.end()) {
			return std::nullopt;
		}
		return it->second;
	}

	bool FileStorageProvider::setValue(const std::string& key, const std::string& value) {
		std::lock_guard<std::mutex> lock(_mutex);
		auto values = load();
		values[key] = value;
		return save(values);
	}

	void FileStorageProvider::removeValue(const std::string& key) {
		std::lock_guard<std::mutex> lock(_mutex);
		auto values = load();
		if (values.erase(key) > 0) {
			save(values);
		}
	}

	// --- Windows Credential Manager ------------------------------------------

#ifdef _WIN32
	namespace {

		std::wstring widen(const std::string& utf8) {
			if (utf8.empty()) {
				return {};
			}
			int needed = MultiByteToWideChar(CP_UTF8, 0, utf8.data(),
				static_cast<int>(utf8.size()), nullptr, 0);
			if (needed <= 0) {
				return {};
			}
			std::wstring wide(static_cast<size_t>(needed), L'\0');
			MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()),
				wide.data(), needed);
			return wide;
		}

		/**
		 * IStorageProvider on the Windows Credential Manager. Values live as
		 * generic credentials named "smartthings4cpp/<key>", persisted for the
		 * current user (survives reboots, roams with the profile where enabled).
		 * The CredMan API is internally synchronized, so no extra locking.
		 */
		class WindowsCredentialStorageProvider : public IStorageProvider {
		public:
			std::optional<std::string> getValue(const std::string& key) override {
				PCREDENTIALW credential = nullptr;
				if (!CredReadW(targetFor(key).c_str(), CRED_TYPE_GENERIC, 0, &credential)) {
					return std::nullopt;
				}
				std::string value;
				if (credential->CredentialBlob && credential->CredentialBlobSize > 0) {
					value.assign(reinterpret_cast<const char*>(credential->CredentialBlob),
						credential->CredentialBlobSize);
				}
				CredFree(credential);
				return value;
			}

			bool setValue(const std::string& key, const std::string& value) override {
				std::wstring target = targetFor(key);
				CREDENTIALW credential{};
				credential.Type = CRED_TYPE_GENERIC;
				credential.TargetName = target.data();
				credential.CredentialBlob = reinterpret_cast<LPBYTE>(
					const_cast<char*>(value.data()));
				credential.CredentialBlobSize = static_cast<DWORD>(value.size());
				credential.Persist = CRED_PERSIST_LOCAL_MACHINE;
				return CredWriteW(&credential, 0) != FALSE;
			}

			void removeValue(const std::string& key) override {
				CredDeleteW(targetFor(key).c_str(), CRED_TYPE_GENERIC, 0);
			}

		private:
			static std::wstring targetFor(const std::string& key) {
				return L"smartthings4cpp/" + widen(key);
			}
		};

	} // namespace
#endif // _WIN32

	// --- factories ------------------------------------------------------------

	std::unique_ptr<IStorageProvider> makeDefaultStorageProvider(const std::filesystem::path& workingDir) {
		return std::make_unique<FileStorageProvider>(workingDir / "storage.json");
	}

	std::unique_ptr<IStorageProvider> makeDefaultKeychainProvider(const std::filesystem::path& workingDir) {
#ifdef _WIN32
		(void)workingDir;
		return std::make_unique<WindowsCredentialStorageProvider>();
#else
		// Native macOS Keychain / libsecret backends are planned; until then
		// secrets share the file mechanism (documented in storage.h).
		return std::make_unique<FileStorageProvider>(workingDir / "keychain.json");
#endif
	}

} // namespace smartthings4cpp
