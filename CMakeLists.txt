#
# EagleSat 2 Ground Station Telemetry & Telecommunications System Build Configuration.
# CMake minimum version must be at least 3.4
# CSDK assumes a minimum language standard of C99.
#

cmake_minimum_required(VERSION 3.4)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(estts)

# Configuration options
# Use the -D argument on command line

option(
        TI_DEV_MODE
        "Set to ON to initialize the transmission interface with no real serial interface"
        false
)
option(
        ESTTS_COMMAND_STREAM_MODE
        "Set to ON to initialize ESTTS in command stream mode - commands sent as batch as opposed to individually"
        false
)
option(
        
        "Set to ON to initialize ESTTS in command stream mode - commands sent as batch as opposed to individually"
        false
)


message(STATUS "#### Build Configuration ####")
message(STATUS "TI_DEV_MODE: ${TI_DEV_MODE}")
message(STATUS "ESTTS_COMMAND_STREAM_MODE: ${ESTTS_COMMAND_STREAM_MODE}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "#### End Build Configuration ####")

add_subdirectory(src)
add_subdirectory(runtime)
add_subdirectory(tests/estts-demo1)
add_subdirectory(tests/estts-sapi-test)
add_subdirectory(tests/estts-telem-handler-test)
add_subdirectory(tests/estts-acs-tests)
add_subdirectory(lib/spdlog)
add_subdirectory(lib/json)
add_subdirectory(tests/estts-cosmos-demo1)
