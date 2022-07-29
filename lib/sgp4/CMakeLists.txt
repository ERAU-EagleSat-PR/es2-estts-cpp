

CMAKE_MINIMUM_REQUIRED(VERSION 3.13.4)
PROJECT(SGP4)

if (POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion")
endif(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(
        SRCS
        libsgp4/CoordGeodetic.cc
        libsgp4/CoordTopocentric.cc
        libsgp4/DateTime.cc
        libsgp4/DecayedException.cc
        libsgp4/Eci.cc
        libsgp4/Globals.cc
        libsgp4/Observer.cc
        libsgp4/OrbitalElements.cc
        libsgp4/SGP4.cc
        libsgp4/SatelliteException.cc
        libsgp4/SolarPosition.cc
        libsgp4/TimeSpan.cc
        libsgp4/Tle.cc
        libsgp4/TleException.cc
        libsgp4/Util.cc
        libsgp4/Vector.cc
)

set(
        INCS
        ${CMAKE_CURRENT_SOURCE_DIR}/libsgp4
)

add_library(sgp4 STATIC ${SRCS})
target_include_directories(
        sgp4
        PUBLIC
        ${INCS}
)
add_library(sgp4s SHARED ${SRCS})
target_include_directories(
        sgp4
        PUBLIC
        ${INCS}
)

include_directories(libsgp4)

add_subdirectory(libsgp4)
add_subdirectory(sattrack)
add_subdirectory(runtest)
add_subdirectory(passpredict)

file(COPY SGP4-VER.TLE DESTINATION ${PROJECT_BINARY_DIR})