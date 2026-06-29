# Examples CMakeLists.txt

# Device listing example (connect + list devices and capabilities)
add_executable(device_listing device_listing.cpp)
target_link_libraries(device_listing PRIVATE smartthings4cpp)
set_target_properties(device_listing PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Authentication example
add_executable(authentication authentication.cpp)
target_link_libraries(authentication PRIVATE smartthings4cpp)
set_target_properties(authentication PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
