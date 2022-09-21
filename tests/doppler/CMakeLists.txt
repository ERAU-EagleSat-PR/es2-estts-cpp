#
# doppler build configuration.
#

# Add primary executable for estts-esttc-command-test
add_executable(doppler src/main.cpp)

target_include_directories(
        doppler
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(doppler PUBLIC estts)
target_link_libraries(doppler PUBLIC sgp4)