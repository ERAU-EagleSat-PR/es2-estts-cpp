#
# estts-eps-voltage-test build configuration.
#

# Add primary executable for estts-tnc_emulator-test
add_executable(estts-eps-voltage-test src/main.cpp)

target_include_directories(
        estts-eps-voltage-test
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(estts-eps-voltage-test PUBLIC estts)