# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build

# Include any dependencies generated for this target.
include demo/CMakeFiles/lgf_demo.dir/depend.make

# Include the progress variables for this target.
include demo/CMakeFiles/lgf_demo.dir/progress.make

# Include the compile flags for this target's objects.
include demo/CMakeFiles/lgf_demo.dir/flags.make

demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o: demo/CMakeFiles/lgf_demo.dir/flags.make
demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o: ../demo/lgf_demo.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o"
	cd /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build/demo && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/lgf_demo.dir/lgf_demo.cc.o -c /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/demo/lgf_demo.cc

demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lgf_demo.dir/lgf_demo.cc.i"
	cd /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build/demo && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/demo/lgf_demo.cc > CMakeFiles/lgf_demo.dir/lgf_demo.cc.i

demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lgf_demo.dir/lgf_demo.cc.s"
	cd /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build/demo && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/demo/lgf_demo.cc -o CMakeFiles/lgf_demo.dir/lgf_demo.cc.s

demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o.requires:

.PHONY : demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o.requires

demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o.provides: demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o.requires
	$(MAKE) -f demo/CMakeFiles/lgf_demo.dir/build.make demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o.provides.build
.PHONY : demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o.provides

demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o.provides.build: demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o


# Object files for target lgf_demo
lgf_demo_OBJECTS = \
"CMakeFiles/lgf_demo.dir/lgf_demo.cc.o"

# External object files for target lgf_demo
lgf_demo_EXTERNAL_OBJECTS =

demo/lgf_demo: demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o
demo/lgf_demo: demo/CMakeFiles/lgf_demo.dir/build.make
demo/lgf_demo: lemon/libemon.a
demo/lgf_demo: demo/CMakeFiles/lgf_demo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable lgf_demo"
	cd /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build/demo && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lgf_demo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
demo/CMakeFiles/lgf_demo.dir/build: demo/lgf_demo

.PHONY : demo/CMakeFiles/lgf_demo.dir/build

demo/CMakeFiles/lgf_demo.dir/requires: demo/CMakeFiles/lgf_demo.dir/lgf_demo.cc.o.requires

.PHONY : demo/CMakeFiles/lgf_demo.dir/requires

demo/CMakeFiles/lgf_demo.dir/clean:
	cd /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build/demo && $(CMAKE_COMMAND) -P CMakeFiles/lgf_demo.dir/cmake_clean.cmake
.PHONY : demo/CMakeFiles/lgf_demo.dir/clean

demo/CMakeFiles/lgf_demo.dir/depend:
	cd /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1 /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/demo /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build/demo /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build/demo/CMakeFiles/lgf_demo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : demo/CMakeFiles/lgf_demo.dir/depend
