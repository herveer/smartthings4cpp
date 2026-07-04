#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

/**
 * @file storage.h
 * @brief Key/value persistence interfaces used by the OAuth-mode Client
 *
 * An OAuth Client needs to remember things across runs: the token pair (so the
 * second run authenticates silently via the refresh token, no browser) and the
 * installed-app id. Two IStorageProvider instances cover that with different
 * sensitivity levels:
 *  - a "keychain" provider for secrets (tokens) - the default is the OS
 *    credential store where implemented (Windows Credential Manager),
 *  - a plain "storage" provider for non-secrets (installed-app id) - the
 *    default is a JSON file in the Client's working directory.
 * Inject your own implementations through Client's DI constructor to use a
 * database, your app's settings system, another OS keychain, etc.
 */

namespace smartthings4cpp {

	/**
	 * @brief Minimal key/value persistence interface
	 *
	 * Implementations must be safe to call from multiple threads (the Client
	 * touches storage from the caller's thread and from the embedded webhook
	 * server's thread).
	 */
	class IStorageProvider {
	public:
		virtual ~IStorageProvider() = default;

		/**
		 * @brief Read a value
		 * @return The stored value, or std::nullopt if the key is absent
		 */
		virtual std::optional<std::string> getValue(const std::string& key) = 0;

		/**
		 * @brief Store (or overwrite) a value
		 * @return true if the value was persisted
		 */
		virtual bool setValue(const std::string& key, const std::string& value) = 0;

		/** @brief Remove a key (no-op if absent). */
		virtual void removeValue(const std::string& key) = 0;
	};

	/**
	 * @brief IStorageProvider backed by a single JSON file
	 *
	 * Each operation re-reads / rewrites the file (under an internal mutex), so
	 * two providers pointed at the same file see each other's writes. A missing
	 * or unreadable file behaves as empty. Parent directories are created on
	 * first write.
	 */
	class FileStorageProvider : public IStorageProvider {
	public:
		/** @param filePath Full path of the JSON file to persist into */
		explicit FileStorageProvider(std::filesystem::path filePath);

		std::optional<std::string> getValue(const std::string& key) override;
		bool setValue(const std::string& key, const std::string& value) override;
		void removeValue(const std::string& key) override;

	private:
		std::filesystem::path _filePath;
		std::mutex _mutex;

		std::map<std::string, std::string> load() const;
		bool save(const std::map<std::string, std::string>& values) const;
	};

	/**
	 * @brief Default non-secret storage: a "storage.json" file in @p workingDir
	 */
	std::unique_ptr<IStorageProvider> makeDefaultStorageProvider(const std::filesystem::path& workingDir);

	/**
	 * @brief Default secret storage
	 *
	 * Windows: the Windows Credential Manager (entries named
	 * @c smartthings4cpp/<key>, visible in the "Credential Manager" control
	 * panel). Other platforms: falls back to a "keychain.json" file in
	 * @p workingDir for now - native macOS Keychain / libsecret backends are a
	 * planned follow-up, and the file fallback keeps the API identical until
	 * then. Inject your own IStorageProvider if you need something stronger.
	 * @param workingDir Only used by the file fallback (ignored on Windows)
	 */
	std::unique_ptr<IStorageProvider> makeDefaultKeychainProvider(const std::filesystem::path& workingDir);

} // namespace smartthings4cpp
