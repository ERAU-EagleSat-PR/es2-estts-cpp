#
# cosmos-groundstation-range-testbed build configuration.
#

# Add primary executable for cosmos-groundstation-range-testbed
add_executable(cosmos-groundstation-range-testbed src/main.cpp)

target_include_directories(
    cosmos-groundstation-range-testbed
    PUBLIC
    ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(cosmos-groundstation-range-testbed PUBLIC estts)