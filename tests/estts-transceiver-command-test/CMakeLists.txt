#
# estts-transceiver-command-test build configuration.
#

# Add primary executable for estts-transceiver-command-test
add_executable(estts-transceiver-command-test src/main.cpp)

target_include_directories(
        estts-transceiver-command-test
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(estts-transceiver-command-test PUBLIC estts)