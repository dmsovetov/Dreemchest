# Set the minimum CMake version
cmake_minimum_required(VERSION 3.2)

# Include CMake globals
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../CMake")
include(Globals)

# Declare the project
project(Dreemchest)

# Add multicore build flag when generating for MSVC
if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif (MSVC)

# Available options
option(DC_BUILD_EXAMPLES "Build Dreemchest examples" ON)
option(DC_BUILD_TESTS "Build Dreemchest tests" OFF)
option(DC_OPENGL_ENABLED "Build with OpenGL support" ON)
option(DC_BOX2D_ENABLED "Build with Box2D support" OFF)
option(DC_SOUND_ENABLED "Build with sound support" OFF)
option(DC_COMPOSER_ENABLED "Build Dreemchest Composer" OFF)
set(DC_QT_SUPPORT "Disabled" CACHE STRING "Build with Qt support Disabled|Qt4|Qt5")
set_property(CACHE DC_QT_SUPPORT PROPERTY STRINGS "Disabled" "Auto" "Qt4" "Qt5")

# Add include directory
include_directories(Dreemchest)

# Add Nimble subproject
include_directories(Nimble)
add_definitions(-DNIMBLE_NAMESPACE=DC_NAMESPACE)
add_subdirectory(Nimble)

# Setup definitions
if (DC_QT_SUPPORT MATCHES "Auto")
	find_package(Qt4 QUIET)
	find_package(Qt5Widgets QUIET)
	
	if (Qt4_FOUND AND Qt5Widgets_FOUND)
		message(FATAL_ERROR "Both Qt4 and Qt5 found, you should use manually which version to use.")
	endif (Qt4_FOUND AND Qt5Widgets_FOUND)
	
	if (Qt4_FOUND)
		set(DC_QT_SUPPORT "Qt4")
		set(DC_QT_PATH ${QT_BINARY_DIR})
	elseif (Qt5Widgets_FOUND)
		set(DC_QT_SUPPORT "Qt5")
		set(DC_QT_PATH ${_qt5Core_install_prefix}/bin)
	else (Qt4_FOUND)
		message(WARNING "Qt library not found, disabling Qt support.")
		set(DC_QT_SUPPORT "Disabled")
	endif (Qt4_FOUND)
endif (DC_QT_SUPPORT MATCHES "Auto")

if (DC_QT_SUPPORT MATCHES "Qt4")
	add_definitions(-DDC_QT4_ENABLED)
	set(DC_QT_ENABLED ON)
elseif (DC_QT_SUPPORT MATCHES "Qt5")
	add_definitions(-DDC_QT5_ENABLED)
	set(DC_QT_ENABLED ON)
endif (DC_QT_SUPPORT MATCHES "Qt4")

# Setup variables
if (WIN32)
	set(DC_PLATFORM "Windows")
	set(DC_THREADS "Windows")
	add_definitions(-DDC_PLATFORM_WINDOWS -DDC_THREADS_WINDOWS)
elseif (ANDROID)
	set(DC_PLATFORM "Android")
	set(DC_THREADS "Posix")
	set(DC_OPENGL_ENABLED OFF)
	add_definitions(-DDC_PLATFORM_ANDROID -DDC_THREADS_POSIX)
endif (WIN32)

# Add child projects
add_subdirectory(Dreemchest)

if (DC_WITH_RELIGHT)
	add_definitions(-DHAVE_RELIGHT)
	add_subdirectory(Relight/src)
	include_directories(Relight/src)
endif ()

if (DC_BUILD_EXAMPLES)
	add_subdirectory(Examples)
endif (DC_BUILD_EXAMPLES)

if (DC_BUILD_TESTS)
	find_package(GTest REQUIRED)
	add_subdirectory(Tests)
endif (DC_BUILD_TESTS)

if (DC_COMPOSER_ENABLED)
	if (NOT DC_QT_ENABLED)
		message(FATAL_ERROR "Dreemchest Composer build requested but no Qt found.")
	endif (NOT DC_QT_ENABLED)
	
	add_subdirectory(Composer)
endif (DC_COMPOSER_ENABLED)