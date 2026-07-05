# Library source files
set(SMARTTHINGS4CPP_SOURCES
    client.cpp
    device.cpp
    capability.cpp
    webhook.cpp
    http_client.cpp
    http_server.cpp
    storage.cpp
    json_utils.cpp
    base64.cpp
    oauth2/oauth2_authenticator.cpp
    # UnknownCapability is the null-object fallback returned by createCapability()
    # for ids without a generated class; it is infrastructure, not a schema class.
    capabilities/unknown_capability.cpp
    # Generated capabilities (standard + Samsung-proprietary): one TU per namespace
    # group plus the two factories. This block is rewritten by
    # tools/generate_capabilities.ps1 - do not edit the entries by hand.
    # === BEGIN GENERATED CAPABILITY SOURCES (managed by tools/generate_capabilities.ps1) ===
    capabilities/standard.cpp
    capabilities/samsungce.cpp
    capabilities/samsungvd.cpp
    capabilities/custom.cpp
    capabilities/sec.cpp
    capabilities/hca.cpp
    capabilities/samsungim.cpp
    capabilities/standard_factory.cpp
    capabilities/proprietary_factory.cpp
    # === END GENERATED CAPABILITY SOURCES ===
)

# Create the library
add_library(smartthings4cpp ${SMARTTHINGS4CPP_SOURCES})

# Set library properties
set_target_properties(smartthings4cpp PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include directories
target_include_directories(smartthings4cpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(smartthings4cpp
    PUBLIC
        cpr::cpr
        nlohmann_json::nlohmann_json
        ReactiveLitepp::ReactiveLitepp
        Threads::Threads
    PRIVATE
        # Embedded webhook/OAuth server - implementation detail of
        # http_server.cpp only (never appears in a public header).
        httplib::httplib
)

if (WIN32)
    # advapi32: Windows Credential Manager (Cred* APIs) used by storage.cpp.
    target_link_libraries(smartthings4cpp PUBLIC ws2_32 PRIVATE advapi32)
    # windows.h (pulled in transitively via cpr/curl) defines max/min macros
    # that shadow std::max/std::min unless this is set - needed for the
    # polling interval clamp in client.cpp. PUBLIC so consumers (examples,
    # tests) that also happen to include windows.h transitively are protected too.
    target_compile_definitions(smartthings4cpp PUBLIC NOMINMAX)
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(smartthings4cpp PRIVATE /W4)
else()
    target_compile_options(smartthings4cpp PRIVATE -Wall -Wextra -Wpedantic)
endif()
