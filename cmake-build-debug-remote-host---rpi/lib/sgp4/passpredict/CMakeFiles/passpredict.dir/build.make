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
include lib/sgp4/passpredict/CMakeFiles/passpredict.dir/depend.make

# Include the progress variables for this target.
include lib/sgp4/passpredict/CMakeFiles/passpredict.dir/progress.make

# Include the compile flags for this target's objects.
include lib/sgp4/passpredict/CMakeFiles/passpredict.dir/flags.make

lib/sgp4/passpredict/CMakeFiles/passpredict.dir/passpredict.cc.o: lib/sgp4/passpredict/CMakeFiles/passpredict.dir/flags.make
lib/sgp4/passpredict/CMakeFiles/passpredict.dir/passpredict.cc.o: ../lib/sgp4/passpredict/passpredict.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/sgp4/passpredict/CMakeFiles/passpredict.dir/passpredict.cc.o"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4/passpredict && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/passpredict.dir/passpredict.cc.o -c /tmp/es2-estts-cpp/lib/sgp4/passpredict/passpredict.cc

lib/sgp4/passpredict/CMakeFiles/passpredict.dir/passpredict.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/passpredict.dir/passpredict.cc.i"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4/passpredict && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/es2-estts-cpp/lib/sgp4/passpredict/passpredict.cc > CMakeFiles/passpredict.dir/passpredict.cc.i

lib/sgp4/passpredict/CMakeFiles/passpredict.dir/passpredict.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/passpredict.dir/passpredict.cc.s"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4/passpredict && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/es2-estts-cpp/lib/sgp4/passpredict/passpredict.cc -o CMakeFiles/passpredict.dir/passpredict.cc.s

# Object files for target passpredict
passpredict_OBJECTS = \
"CMakeFiles/passpredict.dir/passpredict.cc.o"

# External object files for target passpredict
passpredict_EXTERNAL_OBJECTS =

lib/sgp4/passpredict/passpredict: lib/sgp4/passpredict/CMakeFiles/passpredict.dir/passpredict.cc.o
lib/sgp4/passpredict/passpredict: lib/sgp4/passpredict/CMakeFiles/passpredict.dir/build.make
lib/sgp4/passpredict/passpredict: lib/sgp4/libsgp4.a
lib/sgp4/passpredict/passpredict: lib/sgp4/passpredict/CMakeFiles/passpredict.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable passpredict"
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4/passpredict && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/passpredict.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/sgp4/passpredict/CMakeFiles/passpredict.dir/build: lib/sgp4/passpredict/passpredict

.PHONY : lib/sgp4/passpredict/CMakeFiles/passpredict.dir/build

lib/sgp4/passpredict/CMakeFiles/passpredict.dir/clean:
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4/passpredict && $(CMAKE_COMMAND) -P CMakeFiles/passpredict.dir/cmake_clean.cmake
.PHONY : lib/sgp4/passpredict/CMakeFiles/passpredict.dir/clean

lib/sgp4/passpredict/CMakeFiles/passpredict.dir/depend:
	cd /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/es2-estts-cpp /tmp/es2-estts-cpp/lib/sgp4/passpredict /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4/passpredict /tmp/es2-estts-cpp/cmake-build-debug-remote-host---rpi/lib/sgp4/passpredict/CMakeFiles/passpredict.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/sgp4/passpredict/CMakeFiles/passpredict.dir/depend
