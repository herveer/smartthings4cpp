#include <catch2/catch_test_macros.hpp>

#include "smartthings4cpp/types.h"
#include "smartthings4cpp/exceptions.h"

using namespace smartthings4cpp;

TEST_CASE("Result<T> reports success and carries a value", "[types]") {
	Result<int> ok(42);
	REQUIRE(ok.isSuccess());
	REQUIRE(ok.hasValue());
	REQUIRE(ok.value.value() == 42);
	REQUIRE(static_cast<bool>(ok));
}

TEST_CASE("Result<T> reports failure", "[types]") {
	Result<int> err(ErrorCode::NetworkError, "boom");
	REQUIRE_FALSE(err.isSuccess());
	REQUIRE_FALSE(err.hasValue());
	REQUIRE(err.error == ErrorCode::NetworkError);
	REQUIRE(err.error_message == "boom");
	REQUIRE_FALSE(static_cast<bool>(err));
}

TEST_CASE("Result<void> reports success and failure", "[types]") {
	Result<void> ok;
	REQUIRE(ok.isSuccess());

	Result<void> err(ErrorCode::AuthenticationFailed, "bad token");
	REQUIRE_FALSE(err.isSuccess());
	REQUIRE(err.error == ErrorCode::AuthenticationFailed);
	REQUIRE(err.error_message == "bad token");
}

TEST_CASE("errorCodeToString covers the error codes", "[types]") {
	REQUIRE(errorCodeToString(ErrorCode::Success) == "Success");
	REQUIRE(errorCodeToString(ErrorCode::AuthenticationFailed) == "Authentication Failed");
	REQUIRE(errorCodeToString(ErrorCode::RateLimited) == "Rate Limited");
	REQUIRE(errorCodeToString(ErrorCode::ApiError) == "API Error");
}
