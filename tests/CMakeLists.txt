# Find Catch2
find_package(Catch2 3 CONFIG REQUIRED)

# Test sources
set(TEST_SOURCES
    main.cpp
    client_tests.cpp
    device_tests.cpp
    capability_tests.cpp
    json_utils_tests.cpp
    types_tests.cpp
    oauth2_authenticator_tests.cpp
    webhook_tests.cpp
    webhook_signature_tests.cpp
    storage_tests.cpp
    http_server_tests.cpp
)

# Create test executable
add_executable(smartthings4cpp_tests ${TEST_SOURCES})

# Link with library and Catch2
target_link_libraries(smartthings4cpp_tests
    PRIVATE
        smartthings4cpp
        Catch2::Catch2WithMain
)

# Set C++ standard
set_target_properties(smartthings4cpp_tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include Catch2 CMake modules
include(CTest)
include(Catch)

# Register tests with CTest
catch_discover_tests(smartthings4cpp_tests)

# Add test target
add_test(NAME smartthings4cpp_tests COMMAND smartthings4cpp_tests)
