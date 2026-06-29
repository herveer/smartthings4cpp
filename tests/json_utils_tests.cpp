#include <catch2/catch_test_macros.hpp>

#include "smartthings4cpp/json_utils.h"

using namespace smartthings4cpp;

TEST_CASE("json_utils::parse parses valid JSON and throws on invalid", "[json_utils]") {
	auto json = json_utils::parse(R"({"a":1,"b":"two"})");
	REQUIRE(json["a"].get<int>() == 1);
	REQUIRE(json["b"].get<std::string>() == "two");

	REQUIRE_THROWS_AS(json_utils::parse("{not valid"), JsonParseException);
}

TEST_CASE("json_utils::getValue returns nullopt on missing or wrong type", "[json_utils]") {
	auto json = json_utils::parse(R"({"name":"lamp","count":3})");

	REQUIRE(json_utils::getValue<std::string>(json, "name").value() == "lamp");
	REQUIRE(json_utils::getValue<int>(json, "count").value() == 3);
	REQUIRE_FALSE(json_utils::getValue<std::string>(json, "missing").has_value());
	// Wrong type: "count" is not a string.
	REQUIRE_FALSE(json_utils::getValue<std::string>(json, "count").has_value());
}

TEST_CASE("json_utils::getValueOr falls back to the default", "[json_utils]") {
	auto json = json_utils::parse(R"({"name":"lamp"})");
	REQUIRE(json_utils::getValueOr<std::string>(json, "name", "x") == "lamp");
	REQUIRE(json_utils::getValueOr<std::string>(json, "missing", "fallback") == "fallback");
}

TEST_CASE("json_utils::getRequiredValue throws when key is absent", "[json_utils]") {
	auto json = json_utils::parse(R"({"name":"lamp"})");
	REQUIRE(json_utils::getRequiredValue<std::string>(json, "name") == "lamp");
	REQUIRE_THROWS_AS(json_utils::getRequiredValue<std::string>(json, "missing"), JsonParseException);
}

TEST_CASE("json_utils::hasRequiredKeys checks every key", "[json_utils]") {
	auto json = json_utils::parse(R"({"a":1,"b":2})");
	REQUIRE(json_utils::hasRequiredKeys(json, {"a", "b"}));
	REQUIRE_FALSE(json_utils::hasRequiredKeys(json, {"a", "c"}));
}
