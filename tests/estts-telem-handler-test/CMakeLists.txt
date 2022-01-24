#
# estts-telem-handler-test build configuration.
#

# Add primary executable for estts-sapi-test
add_executable(estts-telem-handler-test src/main.cpp)

target_include_directories(
        estts-telem-handler-test
        PUBLIC
        ${estts_include_directories}
)

# Link estts library to build
target_link_libraries(estts-telem-handler-test PUBLIC estts)