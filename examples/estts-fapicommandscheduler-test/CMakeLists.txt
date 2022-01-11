#
# estts-demo1 build configuration.
#

# Add primary executable for estts-demo1
add_executable(estts-fapicommandscheduler-test src/main.cpp)

target_include_directories(
        estts-fapicommandscheduler-test
        PUBLIC
        ${estts_include_directories}
)

# Link GSTTS library to build
target_link_libraries(estts-fapicommandscheduler-test PUBLIC estts)