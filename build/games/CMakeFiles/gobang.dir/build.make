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
CMAKE_SOURCE_DIR = /home/wjc/src/mycode/homework/algo/gobang

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wjc/src/mycode/homework/algo/gobang/build

# Include any dependencies generated for this target.
include games/CMakeFiles/gobang.dir/depend.make

# Include the progress variables for this target.
include games/CMakeFiles/gobang.dir/progress.make

# Include the compile flags for this target's objects.
include games/CMakeFiles/gobang.dir/flags.make

games/CMakeFiles/gobang.dir/gobang.cpp.o: games/CMakeFiles/gobang.dir/flags.make
games/CMakeFiles/gobang.dir/gobang.cpp.o: ../games/gobang.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wjc/src/mycode/homework/algo/gobang/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object games/CMakeFiles/gobang.dir/gobang.cpp.o"
	cd /home/wjc/src/mycode/homework/algo/gobang/build/games && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gobang.dir/gobang.cpp.o -c /home/wjc/src/mycode/homework/algo/gobang/games/gobang.cpp

games/CMakeFiles/gobang.dir/gobang.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gobang.dir/gobang.cpp.i"
	cd /home/wjc/src/mycode/homework/algo/gobang/build/games && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wjc/src/mycode/homework/algo/gobang/games/gobang.cpp > CMakeFiles/gobang.dir/gobang.cpp.i

games/CMakeFiles/gobang.dir/gobang.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gobang.dir/gobang.cpp.s"
	cd /home/wjc/src/mycode/homework/algo/gobang/build/games && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wjc/src/mycode/homework/algo/gobang/games/gobang.cpp -o CMakeFiles/gobang.dir/gobang.cpp.s

games/CMakeFiles/gobang.dir/gobang.cpp.o.requires:

.PHONY : games/CMakeFiles/gobang.dir/gobang.cpp.o.requires

games/CMakeFiles/gobang.dir/gobang.cpp.o.provides: games/CMakeFiles/gobang.dir/gobang.cpp.o.requires
	$(MAKE) -f games/CMakeFiles/gobang.dir/build.make games/CMakeFiles/gobang.dir/gobang.cpp.o.provides.build
.PHONY : games/CMakeFiles/gobang.dir/gobang.cpp.o.provides

games/CMakeFiles/gobang.dir/gobang.cpp.o.provides.build: games/CMakeFiles/gobang.dir/gobang.cpp.o


# Object files for target gobang
gobang_OBJECTS = \
"CMakeFiles/gobang.dir/gobang.cpp.o"

# External object files for target gobang
gobang_EXTERNAL_OBJECTS =

bin/gobang: games/CMakeFiles/gobang.dir/gobang.cpp.o
bin/gobang: games/CMakeFiles/gobang.dir/build.make
bin/gobang: games/CMakeFiles/gobang.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wjc/src/mycode/homework/algo/gobang/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/gobang"
	cd /home/wjc/src/mycode/homework/algo/gobang/build/games && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gobang.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
games/CMakeFiles/gobang.dir/build: bin/gobang

.PHONY : games/CMakeFiles/gobang.dir/build

games/CMakeFiles/gobang.dir/requires: games/CMakeFiles/gobang.dir/gobang.cpp.o.requires

.PHONY : games/CMakeFiles/gobang.dir/requires

games/CMakeFiles/gobang.dir/clean:
	cd /home/wjc/src/mycode/homework/algo/gobang/build/games && $(CMAKE_COMMAND) -P CMakeFiles/gobang.dir/cmake_clean.cmake
.PHONY : games/CMakeFiles/gobang.dir/clean

games/CMakeFiles/gobang.dir/depend:
	cd /home/wjc/src/mycode/homework/algo/gobang/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wjc/src/mycode/homework/algo/gobang /home/wjc/src/mycode/homework/algo/gobang/games /home/wjc/src/mycode/homework/algo/gobang/build /home/wjc/src/mycode/homework/algo/gobang/build/games /home/wjc/src/mycode/homework/algo/gobang/build/games/CMakeFiles/gobang.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : games/CMakeFiles/gobang.dir/depend

