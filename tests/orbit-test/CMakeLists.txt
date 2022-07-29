#
# orbit-test build configuration.
#

# Add primary executable for orbit-test
add_executable(orbit-test src/main.cpp)

target_include_directories(
        orbit-test
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(orbit-test PUBLIC estts)
target_link_libraries(orbit-test PUBLIC sgp4)