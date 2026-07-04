#include <catch2/catch_test_macros.hpp>

#include "smartthings4cpp/storage.h"

#include <filesystem>
#include <random>
#include <string>

using namespace smartthings4cpp;

namespace {

	// A fresh directory under the system temp dir, removed when the test ends.
	struct TempDir {
		std::filesystem::path path;

		TempDir() {
			static std::mt19937_64 rng{ std::random_device{}() };
			path = std::filesystem::temp_directory_path()
				/ "smartthings4cpp_tests" / std::to_string(rng());
			std::filesystem::create_directories(path);
		}

		~TempDir() {
			std::error_code ec;
			std::filesystem::remove_all(path, ec);
		}
	};

} // namespace

TEST_CASE("FileStorageProvider round-trips values through its file", "[storage]") {
	TempDir dir;
	FileStorageProvider storage(dir.path / "store.json");

	REQUIRE_FALSE(storage.getValue("missing").has_value());

	REQUIRE(storage.setValue("alpha", "one"));
	REQUIRE(storage.setValue("beta", "two"));
	REQUIRE(storage.getValue("alpha").value_or("") == "one");
	REQUIRE(storage.getValue("beta").value_or("") == "two");

	REQUIRE(storage.setValue("alpha", "uno")); // overwrite
	REQUIRE(storage.getValue("alpha").value_or("") == "uno");

	storage.removeValue("beta");
	REQUIRE_FALSE(storage.getValue("beta").has_value());
	storage.removeValue("beta"); // removing an absent key is a no-op
}

TEST_CASE("FileStorageProvider persists across instances", "[storage]") {
	TempDir dir;
	auto file = dir.path / "store.json";

	{
		FileStorageProvider first(file);
		REQUIRE(first.setValue("key", "value"));
	}

	FileStorageProvider second(file);
	REQUIRE(second.getValue("key").value_or("") == "value");
}

TEST_CASE("FileStorageProvider creates missing parent directories", "[storage]") {
	TempDir dir;
	FileStorageProvider storage(dir.path / "nested" / "deeper" / "store.json");

	REQUIRE(storage.setValue("key", "value"));
	REQUIRE(storage.getValue("key").value_or("") == "value");
}

TEST_CASE("makeDefaultStorageProvider persists into the working directory", "[storage]") {
	TempDir dir;

	auto storage = makeDefaultStorageProvider(dir.path);
	REQUIRE(storage->setValue("installedAppId", "IA-1"));

	auto reopened = makeDefaultStorageProvider(dir.path);
	REQUIRE(reopened->getValue("installedAppId").value_or("") == "IA-1");
}

#ifdef _WIN32
TEST_CASE("Default keychain (Windows Credential Manager) round-trips a secret", "[storage]") {
	// Writes a real credential named "smartthings4cpp/test.roundtrip" and
	// removes it again, leaving the machine clean.
	auto keychain = makeDefaultKeychainProvider({});
	const std::string key = "test.roundtrip";

	keychain->removeValue(key); // clean slate if a prior run leaked
	REQUIRE_FALSE(keychain->getValue(key).has_value());

	REQUIRE(keychain->setValue(key, "s3cret-value"));
	REQUIRE(keychain->getValue(key).value_or("") == "s3cret-value");

	REQUIRE(keychain->setValue(key, "rotated")); // overwrite
	REQUIRE(keychain->getValue(key).value_or("") == "rotated");

	keychain->removeValue(key);
	REQUIRE_FALSE(keychain->getValue(key).has_value());
}
#endif
