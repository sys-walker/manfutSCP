# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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
CMAKE_COMMAND = /home/pere/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/203.5981.166/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/pere/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/203.5981.166/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pere/Documentos/ClionProjects/Manfut_signals

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pere/Documentos/ClionProjects/Manfut_signals/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Manfut.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Manfut.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Manfut.dir/flags.make

CMakeFiles/Manfut.dir/Manfut.c.o: CMakeFiles/Manfut.dir/flags.make
CMakeFiles/Manfut.dir/Manfut.c.o: ../Manfut.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pere/Documentos/ClionProjects/Manfut_signals/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/Manfut.dir/Manfut.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Manfut.dir/Manfut.c.o   -c /home/pere/Documentos/ClionProjects/Manfut_signals/Manfut.c

CMakeFiles/Manfut.dir/Manfut.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Manfut.dir/Manfut.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pere/Documentos/ClionProjects/Manfut_signals/Manfut.c > CMakeFiles/Manfut.dir/Manfut.c.i

CMakeFiles/Manfut.dir/Manfut.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Manfut.dir/Manfut.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pere/Documentos/ClionProjects/Manfut_signals/Manfut.c -o CMakeFiles/Manfut.dir/Manfut.c.s

# Object files for target Manfut
Manfut_OBJECTS = \
"CMakeFiles/Manfut.dir/Manfut.c.o"

# External object files for target Manfut
Manfut_EXTERNAL_OBJECTS =

Manfut: CMakeFiles/Manfut.dir/Manfut.c.o
Manfut: CMakeFiles/Manfut.dir/build.make
Manfut: CMakeFiles/Manfut.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pere/Documentos/ClionProjects/Manfut_signals/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable Manfut"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Manfut.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Manfut.dir/build: Manfut

.PHONY : CMakeFiles/Manfut.dir/build

CMakeFiles/Manfut.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Manfut.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Manfut.dir/clean

CMakeFiles/Manfut.dir/depend:
	cd /home/pere/Documentos/ClionProjects/Manfut_signals/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pere/Documentos/ClionProjects/Manfut_signals /home/pere/Documentos/ClionProjects/Manfut_signals /home/pere/Documentos/ClionProjects/Manfut_signals/cmake-build-debug /home/pere/Documentos/ClionProjects/Manfut_signals/cmake-build-debug /home/pere/Documentos/ClionProjects/Manfut_signals/cmake-build-debug/CMakeFiles/Manfut.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Manfut.dir/depend

