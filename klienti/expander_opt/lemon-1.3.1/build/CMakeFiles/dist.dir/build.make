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

# Utility rule file for dist.

# Include the progress variables for this target.
include CMakeFiles/dist.dir/progress.make

CMakeFiles/dist: ../html
	cmake -E remove_directory lemon-1.3.1
	hg archive lemon-1.3.1
	cmake -E copy cmake/version.cmake lemon-1.3.1/cmake/version.cmake
	tar -czf lemon-nodoc-1.3.1.tar.gz lemon-1.3.1
	zip -r lemon-nodoc-1.3.1.zip lemon-1.3.1
	cmake -E copy_directory doc/html lemon-1.3.1/doc/html
	tar -czf lemon-1.3.1.tar.gz lemon-1.3.1
	zip -r lemon-1.3.1.zip lemon-1.3.1
	cmake -E copy_directory doc/html lemon-doc-1.3.1
	tar -czf lemon-doc-1.3.1.tar.gz lemon-doc-1.3.1
	zip -r lemon-doc-1.3.1.zip lemon-doc-1.3.1
	cmake -E remove_directory lemon-1.3.1
	cmake -E remove_directory lemon-doc-1.3.1

dist: CMakeFiles/dist
dist: CMakeFiles/dist.dir/build.make

.PHONY : dist

# Rule to build all files generated by this target.
CMakeFiles/dist.dir/build: dist

.PHONY : CMakeFiles/dist.dir/build

CMakeFiles/dist.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/dist.dir/cmake_clean.cmake
.PHONY : CMakeFiles/dist.dir/clean

CMakeFiles/dist.dir/depend:
	cd /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1 /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1 /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build /home/askar/projects/proboj-2017-jar/klienti/edmonds_movement/lemon-1.3.1/build/CMakeFiles/dist.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/dist.dir/depend

