# Set the minimum CMake version
cmake_minimum_required(VERSION 2.6)

# Declare the project
project(Dreemchest)

# Available options
option(DC_BUILD_EXAMPLES "Build Dreemchest examples" ON)
option(DC_BUILD_TESTS "Build Dreemchest tests" ON)
option(DC_OPENGL_ENABLED "Build with OpenGL support" ON)

# Add include directory
include_directories(Dreemchest)

# Setup variables
if (WIN32)
	set(DC_PLATFORM "Windows")
	add_definitions(-DDC_PLATFORM_WINDOWS)
endif (WIN32)

# Add child projects
add_subdirectory(Dreemchest)

if (DC_BUILD_EXAMPLES)
	add_subdirectory(Examples)
endif (DC_BUILD_EXAMPLES)

if (DC_BUILD_TESTS)
	find_package(GTest REQUIRED)
	add_subdirectory(Tests)
endif (DC_BUILD_TESTS)