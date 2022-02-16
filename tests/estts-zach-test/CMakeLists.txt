#
# estts-zach-test build configuration.
#

# Add primary executable for estts-zach-test
add_executable(estts-zach-test src/main.cpp)

target_include_directories(
        estts-zach-test
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(estts-zach-test PUBLIC estts)