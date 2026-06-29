# Library source files
set(SMARTTHINGS4CPP_SOURCES
    client.cpp
    device.cpp
    http_client.cpp
    json_utils.cpp
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
)

if (WIN32)
    target_link_libraries(smartthings4cpp PUBLIC ws2_32)
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(smartthings4cpp PRIVATE /W4)
else()
    target_compile_options(smartthings4cpp PRIVATE -Wall -Wextra -Wpedantic)
endif()
