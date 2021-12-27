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
        "Set to initialize the transmission interface with no real serial interface"
        false
)

message(STATUS "#### Build Configuration ####")
message(STATUS "TI_DEV_MODE: ${TI_DEV_MODE}")

add_subdirectory(src)
add_subdirectory(examples/estts-demo1)
add_subdirectory(examples/estts-sapi-test)
add_subdirectory(examples/estts-fapicommandhandler-test)
add_subdirectory(lib/spdlog)