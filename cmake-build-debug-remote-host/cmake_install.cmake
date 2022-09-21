# Install script for directory: /tmp/tmp.cvIXXKse5W

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/tmp/tmp.cvIXXKse5W/cmake-build-debug-remote-host/src/cmake_install.cmake")
  include("/tmp/tmp.cvIXXKse5W/cmake-build-debug-remote-host/runtime/cmake_install.cmake")
  include("/tmp/tmp.cvIXXKse5W/cmake-build-debug-remote-host/lib/spdlog/cmake_install.cmake")
  include("/tmp/tmp.cvIXXKse5W/cmake-build-debug-remote-host/lib/json/cmake_install.cmake")
  include("/tmp/tmp.cvIXXKse5W/cmake-build-debug-remote-host/lib/sgp4/cmake_install.cmake")
  include("/tmp/tmp.cvIXXKse5W/cmake-build-debug-remote-host/tests/serial-test/cmake_install.cmake")
  include("/tmp/tmp.cvIXXKse5W/cmake-build-debug-remote-host/tests/cosmos-groundstation-range-testbed/cmake_install.cmake")
  include("/tmp/tmp.cvIXXKse5W/cmake-build-debug-remote-host/tests/doppler/cmake_install.cmake")
  include("/tmp/tmp.cvIXXKse5W/cmake-build-debug-remote-host/tests/orbit-test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/tmp/tmp.cvIXXKse5W/cmake-build-debug-remote-host/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
