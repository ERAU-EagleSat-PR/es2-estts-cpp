#
# estts-demo1 build configuration.
#

# Add primary executable for estts-demo1
add_executable(estts-fapicommandhandler-test src/main.cpp)

target_include_directories(
        estts-fapicommandhandler-test
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(estts-fapicommandhandler-test PUBLIC estts)