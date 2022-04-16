#
# estts-groundstation-testing build configuration.
#

# Add primary executable for estts-groundstation-testing
add_executable(estts-groundstation-testing src/main.cpp)

target_include_directories(
        estts-groundstation-testing
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(estts-groundstation-testing PUBLIC estts)