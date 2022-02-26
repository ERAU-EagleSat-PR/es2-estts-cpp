#
# estts-demo1 build configuration.
#

# Add primary executable for estts-demo1
add_executable(estts-acs-tests src/main.cpp)

target_include_directories(
        estts-acs-tests
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(estts-acs-tests PUBLIC estts)