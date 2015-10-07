# Set the minimum CMake version
cmake_minimum_required(VERSION 3.2)

# Include CMake globals
include(../CMake/Globals.txt)

# Declare the project
project(Dreemchest)

# Available options
option(DC_BUILD_EXAMPLES "Build Dreemchest examples" ON)
option(DC_BUILD_TESTS "Build Dreemchest tests" OFF)
option(DC_OPENGL_ENABLED "Build with OpenGL support" ON)
option(DC_BOX2D_ENABLED "Build with Box2D support" OFF)
set(DC_QT_SUPPORT "Disabled" CACHE STRING "Build with Qt support Disabled|Qt4|Qt5")
set_property(CACHE DC_QT_SUPPORT PROPERTY STRINGS "Disabled" "Qt4" "Qt5")

# Add include directory
include_directories(Dreemchest)

# Setup definitions
if (DC_QT_SUPPORT MATCHES "Qt4")
	add_definitions(-DDC_QT4_ENABLED)
	
elseif (DC_QT_SUPPORT MATCHES "Qt5")
	add_definitions(-DDC_QT5_ENABLED)
	
endif (DC_QT_SUPPORT MATCHES "Qt4")

# Setup variables
if (WIN32)
	set(DC_PLATFORM "Windows")
	set(DC_THREADS "Windows")
	add_definitions(-DDC_PLATFORM_WINDOWS -DDC_THREADS_WINDOWS)
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