#
# estts-runtime build configuration.
#

# Add primary executable for estts-demo1
add_executable(estts-runtime src/main.cpp)

target_include_directories(
        estts-runtime
        PUBLIC
        ${estts_include_directories}
)

target_link_libraries(estts-runtime PUBLIC estts)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "${CMAKE_BUILD_TYPE} build type detected, setting __DEV__")
    target_compile_definitions(
            estts
            PUBLIC
            __DEV__
    )
    target_compile_options(
            estts
            PRIVATE
    )
endif ()