#
# serial-test build configuration.
#

# Add primary executable for serial-test
add_executable(serial-test src/main.cpp)

target_include_directories(
        serial-test
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
# target_link_libraries(serial-test PUBLIC estts)