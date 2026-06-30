# Library source files
set(SMARTTHINGS4CPP_SOURCES
    client.cpp
    device.cpp
    capability.cpp
    http_client.cpp
    json_utils.cpp
    capabilities/switch.cpp
    capabilities/switch_level.cpp
    capabilities/audio_volume.cpp
    capabilities/audio_mute.cpp
    capabilities/contact_sensor.cpp
    capabilities/temperature_measurement.cpp
    capabilities/thermostat_cooling_setpoint.cpp
    capabilities/refresh.cpp
    capabilities/media_playback.cpp
    capabilities/media_track_control.cpp
    capabilities/media_input_source.cpp
    capabilities/tv_channel.cpp
    capabilities/power_consumption_report.cpp
    capabilities/remote_control_status.cpp
    capabilities/demand_response_load_control.cpp
    capabilities/washer_operating_state.cpp
    capabilities/refrigeration.cpp
    capabilities/execute.cpp
    capabilities/ocf.cpp
    capabilities/unknown_capability.cpp
    # Samsung-proprietary capabilities (generated; one TU per namespace prefix)
    capabilities/samsungce.cpp
    capabilities/samsungvd.cpp
    capabilities/custom.cpp
    capabilities/sec.cpp
    capabilities/hca.cpp
    capabilities/samsungim.cpp
    capabilities/proprietary_factory.cpp
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
