# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /tmp/es2-estts-cpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi

# Include any dependencies generated for this target.
include lib/sgp4/CMakeFiles/sgp4s.dir/depend.make

# Include the progress variables for this target.
include lib/sgp4/CMakeFiles/sgp4s.dir/progress.make

# Include the compile flags for this target's objects.
include lib/sgp4/CMakeFiles/sgp4s.dir/flags.make

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/CoordGeodetic.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/CoordGeodetic.cc.o: ../lib/sgp4/libsgp4/CoordGeodetic.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/CoordGeodetic.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/CoordGeodetic.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/CoordGeodetic.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/CoordGeodetic.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/CoordGeodetic.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/CoordGeodetic.cc > CMakeFiles/sgp4s.dir/libsgp4/CoordGeodetic.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/CoordGeodetic.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/CoordGeodetic.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/CoordGeodetic.cc -o CMakeFiles/sgp4s.dir/libsgp4/CoordGeodetic.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/CoordTopocentric.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/CoordTopocentric.cc.o: ../lib/sgp4/libsgp4/CoordTopocentric.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/CoordTopocentric.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/CoordTopocentric.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/CoordTopocentric.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/CoordTopocentric.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/CoordTopocentric.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/CoordTopocentric.cc > CMakeFiles/sgp4s.dir/libsgp4/CoordTopocentric.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/CoordTopocentric.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/CoordTopocentric.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/CoordTopocentric.cc -o CMakeFiles/sgp4s.dir/libsgp4/CoordTopocentric.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/DateTime.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/DateTime.cc.o: ../lib/sgp4/libsgp4/DateTime.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/DateTime.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/DateTime.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/DateTime.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/DateTime.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/DateTime.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/DateTime.cc > CMakeFiles/sgp4s.dir/libsgp4/DateTime.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/DateTime.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/DateTime.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/DateTime.cc -o CMakeFiles/sgp4s.dir/libsgp4/DateTime.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/DecayedException.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/DecayedException.cc.o: ../lib/sgp4/libsgp4/DecayedException.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/DecayedException.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/DecayedException.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/DecayedException.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/DecayedException.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/DecayedException.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/DecayedException.cc > CMakeFiles/sgp4s.dir/libsgp4/DecayedException.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/DecayedException.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/DecayedException.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/DecayedException.cc -o CMakeFiles/sgp4s.dir/libsgp4/DecayedException.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Eci.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Eci.cc.o: ../lib/sgp4/libsgp4/Eci.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Eci.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/Eci.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Eci.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Eci.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/Eci.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Eci.cc > CMakeFiles/sgp4s.dir/libsgp4/Eci.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Eci.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/Eci.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Eci.cc -o CMakeFiles/sgp4s.dir/libsgp4/Eci.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Globals.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Globals.cc.o: ../lib/sgp4/libsgp4/Globals.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Globals.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/Globals.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Globals.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Globals.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/Globals.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Globals.cc > CMakeFiles/sgp4s.dir/libsgp4/Globals.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Globals.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/Globals.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Globals.cc -o CMakeFiles/sgp4s.dir/libsgp4/Globals.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Observer.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Observer.cc.o: ../lib/sgp4/libsgp4/Observer.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Observer.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/Observer.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Observer.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Observer.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/Observer.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Observer.cc > CMakeFiles/sgp4s.dir/libsgp4/Observer.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Observer.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/Observer.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Observer.cc -o CMakeFiles/sgp4s.dir/libsgp4/Observer.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/OrbitalElements.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/OrbitalElements.cc.o: ../lib/sgp4/libsgp4/OrbitalElements.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/OrbitalElements.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/OrbitalElements.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/OrbitalElements.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/OrbitalElements.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/OrbitalElements.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/OrbitalElements.cc > CMakeFiles/sgp4s.dir/libsgp4/OrbitalElements.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/OrbitalElements.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/OrbitalElements.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/OrbitalElements.cc -o CMakeFiles/sgp4s.dir/libsgp4/OrbitalElements.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SGP4.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SGP4.cc.o: ../lib/sgp4/libsgp4/SGP4.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SGP4.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/SGP4.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/SGP4.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SGP4.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/SGP4.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/SGP4.cc > CMakeFiles/sgp4s.dir/libsgp4/SGP4.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SGP4.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/SGP4.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/SGP4.cc -o CMakeFiles/sgp4s.dir/libsgp4/SGP4.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SatelliteException.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SatelliteException.cc.o: ../lib/sgp4/libsgp4/SatelliteException.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SatelliteException.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/SatelliteException.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/SatelliteException.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SatelliteException.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/SatelliteException.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/SatelliteException.cc > CMakeFiles/sgp4s.dir/libsgp4/SatelliteException.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SatelliteException.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/SatelliteException.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/SatelliteException.cc -o CMakeFiles/sgp4s.dir/libsgp4/SatelliteException.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SolarPosition.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SolarPosition.cc.o: ../lib/sgp4/libsgp4/SolarPosition.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SolarPosition.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/SolarPosition.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/SolarPosition.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SolarPosition.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/SolarPosition.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/SolarPosition.cc > CMakeFiles/sgp4s.dir/libsgp4/SolarPosition.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SolarPosition.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/SolarPosition.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/SolarPosition.cc -o CMakeFiles/sgp4s.dir/libsgp4/SolarPosition.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/TimeSpan.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/TimeSpan.cc.o: ../lib/sgp4/libsgp4/TimeSpan.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/TimeSpan.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/TimeSpan.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/TimeSpan.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/TimeSpan.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/TimeSpan.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/TimeSpan.cc > CMakeFiles/sgp4s.dir/libsgp4/TimeSpan.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/TimeSpan.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/TimeSpan.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/TimeSpan.cc -o CMakeFiles/sgp4s.dir/libsgp4/TimeSpan.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Tle.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Tle.cc.o: ../lib/sgp4/libsgp4/Tle.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Tle.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/Tle.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Tle.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Tle.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/Tle.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Tle.cc > CMakeFiles/sgp4s.dir/libsgp4/Tle.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Tle.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/Tle.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Tle.cc -o CMakeFiles/sgp4s.dir/libsgp4/Tle.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/TleException.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/TleException.cc.o: ../lib/sgp4/libsgp4/TleException.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/TleException.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/TleException.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/TleException.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/TleException.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/TleException.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/TleException.cc > CMakeFiles/sgp4s.dir/libsgp4/TleException.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/TleException.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/TleException.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/TleException.cc -o CMakeFiles/sgp4s.dir/libsgp4/TleException.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Util.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Util.cc.o: ../lib/sgp4/libsgp4/Util.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Util.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/Util.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Util.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Util.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/Util.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Util.cc > CMakeFiles/sgp4s.dir/libsgp4/Util.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Util.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/Util.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Util.cc -o CMakeFiles/sgp4s.dir/libsgp4/Util.cc.s

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Vector.cc.o: lib/sgp4/CMakeFiles/sgp4s.dir/flags.make
lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Vector.cc.o: ../lib/sgp4/libsgp4/Vector.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building CXX object lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Vector.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sgp4s.dir/libsgp4/Vector.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Vector.cc

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Vector.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sgp4s.dir/libsgp4/Vector.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Vector.cc > CMakeFiles/sgp4s.dir/libsgp4/Vector.cc.i

lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Vector.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sgp4s.dir/libsgp4/Vector.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/libsgp4/Vector.cc -o CMakeFiles/sgp4s.dir/libsgp4/Vector.cc.s

# Object files for target sgp4s
sgp4s_OBJECTS = \
"CMakeFiles/sgp4s.dir/libsgp4/CoordGeodetic.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/CoordTopocentric.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/DateTime.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/DecayedException.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/Eci.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/Globals.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/Observer.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/OrbitalElements.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/SGP4.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/SatelliteException.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/SolarPosition.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/TimeSpan.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/Tle.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/TleException.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/Util.cc.o" \
"CMakeFiles/sgp4s.dir/libsgp4/Vector.cc.o"

# External object files for target sgp4s
sgp4s_EXTERNAL_OBJECTS =

lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/CoordGeodetic.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/CoordTopocentric.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/DateTime.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/DecayedException.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Eci.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Globals.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Observer.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/OrbitalElements.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SGP4.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SatelliteException.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/SolarPosition.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/TimeSpan.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Tle.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/TleException.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Util.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/libsgp4/Vector.cc.o
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/build.make
lib/sgp4/libsgp4s.so: lib/sgp4/CMakeFiles/sgp4s.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Linking CXX shared library libsgp4s.so"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sgp4s.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/sgp4/CMakeFiles/sgp4s.dir/build: lib/sgp4/libsgp4s.so

.PHONY : lib/sgp4/CMakeFiles/sgp4s.dir/build

lib/sgp4/CMakeFiles/sgp4s.dir/clean:
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 && $(CMAKE_COMMAND) -P CMakeFiles/sgp4s.dir/cmake_clean.cmake
.PHONY : lib/sgp4/CMakeFiles/sgp4s.dir/clean

lib/sgp4/CMakeFiles/sgp4s.dir/depend:
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/es2-estts-cpp /tmp/es2-estts-cpp/lib/sgp4 /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4 /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4/CMakeFiles/sgp4s.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/sgp4/CMakeFiles/sgp4s.dir/depend

