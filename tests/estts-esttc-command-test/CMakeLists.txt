#
# estts-esttc-command-test build configuration.
#

# Add primary executable for estts-esttc-command-test
add_executable(estts-esttc-command-test src/main.cpp)

target_include_directories(
        estts-esttc-command-test
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(estts-esttc-command-test PUBLIC estts)