# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_SOURCE_DIR = /home/coding/Smart_thread/example

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/coding/Smart_thread/example/build

# Include any dependencies generated for this target.
include CMakeFiles/e6.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/e6.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/e6.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/e6.dir/flags.make

CMakeFiles/e6.dir/e6.cc.o: CMakeFiles/e6.dir/flags.make
CMakeFiles/e6.dir/e6.cc.o: ../e6.cc
CMakeFiles/e6.dir/e6.cc.o: CMakeFiles/e6.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/coding/Smart_thread/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/e6.dir/e6.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/e6.dir/e6.cc.o -MF CMakeFiles/e6.dir/e6.cc.o.d -o CMakeFiles/e6.dir/e6.cc.o -c /home/coding/Smart_thread/example/e6.cc

CMakeFiles/e6.dir/e6.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/e6.dir/e6.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/coding/Smart_thread/example/e6.cc > CMakeFiles/e6.dir/e6.cc.i

CMakeFiles/e6.dir/e6.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/e6.dir/e6.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/coding/Smart_thread/example/e6.cc -o CMakeFiles/e6.dir/e6.cc.s

# Object files for target e6
e6_OBJECTS = \
"CMakeFiles/e6.dir/e6.cc.o"

# External object files for target e6
e6_EXTERNAL_OBJECTS =

e6: CMakeFiles/e6.dir/e6.cc.o
e6: CMakeFiles/e6.dir/build.make
e6: CMakeFiles/e6.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/coding/Smart_thread/example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable e6"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/e6.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/e6.dir/build: e6
.PHONY : CMakeFiles/e6.dir/build

CMakeFiles/e6.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/e6.dir/cmake_clean.cmake
.PHONY : CMakeFiles/e6.dir/clean

CMakeFiles/e6.dir/depend:
	cd /home/coding/Smart_thread/example/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/coding/Smart_thread/example /home/coding/Smart_thread/example /home/coding/Smart_thread/example/build /home/coding/Smart_thread/example/build /home/coding/Smart_thread/example/build/CMakeFiles/e6.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/e6.dir/depend

