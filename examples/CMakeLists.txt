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

# Device control example (read typed state + send a self-restoring command)
add_executable(device_control device_control.cpp)
target_link_libraries(device_control PRIVATE smartthings4cpp)
set_target_properties(device_control PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Full discovery example (dump every capability's attributes + commands per device)
add_executable(full_discovery full_discovery.cpp)
target_link_libraries(full_discovery PRIVATE smartthings4cpp)
set_target_properties(full_discovery PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# OAuth2 authentication example (authorization-code flow: get + refresh tokens)
add_executable(oauth2_authentication oauth2_authentication.cpp)
target_link_libraries(oauth2_authentication PRIVATE smartthings4cpp)
set_target_properties(oauth2_authentication PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Live updates example (automatic background polling + PropertyChanged)
add_executable(live_updates live_updates.cpp)
target_link_libraries(live_updates PRIVATE smartthings4cpp)
set_target_properties(live_updates PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
