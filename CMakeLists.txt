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


message(STATUS "#### Build Configuration ####")
message(STATUS "TI_DEV_MODE: ${TI_DEV_MODE}")
message(STATUS "ESTTS_COMMAND_STREAM_MODE: ${ESTTS_COMMAND_STREAM_MODE}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "ESTTS_BOOST_SERIAL: ${ESTTS_BOOST_SERIAL}")
message(STATUS "#### End Build Configuration ####")

add_subdirectory(src)
add_subdirectory(runtime)
add_subdirectory(lib/spdlog)
add_subdirectory(lib/json)
add_subdirectory(lib/sgp4)
add_subdirectory(tests/serial-test)
add_subdirectory(tests/cosmos-groundstation-range-testbed)

add_subdirectory(tests/orbit-test)