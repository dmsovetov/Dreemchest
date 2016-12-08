# Set the minimum CMake version
cmake_minimum_required(VERSION 3.2)

# Include CMake globals
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../CMake")
include(Globals)
include(FindBundledLibrary)
include(LocateDependency)

# Declare the project
project(Dreemchest)

# Add multicore build flag when generating for MSVC
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif ()

# Available options
option(DC_BUILD_EXAMPLES "Build Dreemchest examples" ON)
option(DC_BUILD_TESTS "Build Dreemchest tests" OFF)
option(DC_OPENGL_ENABLED "Build with OpenGL support" ON)
option(DC_BOX2D_ENABLED "Build with Box2D support" OFF)
option(DC_SOUND_ENABLED "Build with sound support" OFF)
option(DC_COMPOSER_ENABLED "Build Dreemchest Composer" OFF)
option(DC_USE_PCH "Use precompiled headers" OFF)
set(DC_NAMESPACE "Dreemchest" CACHE STRING "Root engine namespace")
set(DC_QT_SUPPORT "Disabled" CACHE STRING "Build with Qt support Disabled|Qt4|Qt5")
set_property(CACHE DC_QT_SUPPORT PROPERTY STRINGS "Disabled" "Auto" "Qt4" "Qt5")

# Set the namespace
add_definitions(-DNIMBLE_NAMESPACE=DC_NAMESPACE)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

# Add include directory
include_directories(Dreemchest)

# Add Nimble subproject
add_subdirectory(Nimble)

# Setup definitions
if (DC_QT_SUPPORT MATCHES "Auto")
    find_package(Qt4 QUIET)
    find_package(Qt5Widgets QUIET)
    
    if (Qt4_FOUND AND Qt5Widgets_FOUND)
        message(FATAL_ERROR "Both Qt4 and Qt5 found, you should use manually which version to use.")
    endif ()
    
    if (Qt4_FOUND)
        set(DC_QT_SUPPORT "Qt4")
        set(DC_QT_PATH ${QT_BINARY_DIR})
    elseif (Qt5Widgets_FOUND)
        set(DC_QT_SUPPORT "Qt5")
        set(DC_QT_PATH ${_qt5Core_install_prefix}/bin)
    else (Qt4_FOUND)
        message(WARNING "Qt library not found, disabling Qt support.")
        set(DC_QT_SUPPORT "Disabled")
    endif ()
endif ()

if (DC_QT_SUPPORT MATCHES "Qt4")
    set(DC_QT_ENABLED ON)
    set(DC_QT_VERSION 4)
elseif (DC_QT_SUPPORT MATCHES "Qt5")
    set(DC_QT_ENABLED ON)
    set(DC_QT_VERSION 5)
endif ()

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
elseif (APPLE)
    set(DC_PLATFORM "MacOS")
    set(DC_THREADS "Posix")
    add_definitions(-DDC_PLATFORM_MACOS -DDC_THREADS_POSIX)
endif ()

# Add child projects
add_subdirectory(Dreemchest)

target_include_directories(
    Dreemchest PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Nimble"
    )

if (DC_WITH_RELIGHT)
    add_definitions(-DHAVE_RELIGHT)
    add_subdirectory(Relight/src)
    include_directories(Relight/src)
endif ()

if (DC_BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif ()

if (DC_BUILD_TESTS)
    find_package(GTest REQUIRED)
    add_subdirectory(Tests)
endif ()

if (DC_COMPOSER_ENABLED)
    if (NOT DC_QT_ENABLED)
        message(FATAL_ERROR "Dreemchest Composer build requested but no Qt found.")
    endif ()
    
    add_subdirectory(Composer)
endif ()

# Add solution folders
if (DC_COMPOSER_ENABLED)
    set_property(TARGET Composer PROPERTY FOLDER "Core")
endif()
set_property(TARGET Dreemchest PROPERTY FOLDER "Core")
set_property(TARGET Nimble PROPERTY FOLDER "Externals")
if (DC_WITH_RELIGHT)
    set_property(TARGET Relight PROPERTY FOLDER "Externals")
endif()
