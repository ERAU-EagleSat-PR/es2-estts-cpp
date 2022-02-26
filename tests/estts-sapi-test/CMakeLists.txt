#
# estts-tnc_emulator-test build configuration.
#

# Add primary executable for estts-tnc_emulator-test
add_executable(estts-sapi-test src/main.cpp)

target_include_directories(
        estts-sapi-test
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(estts-sapi-test PUBLIC estts)