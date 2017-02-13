# Set the target name
set(TARGET_NAME Composer)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Automatically run uic when needed.
set(CMAKE_AUTOUIC ON)

# Automatically run rcc when needed.
set(CMAKE_AUTORCC ON)

# Set the precompiled header name
set(DC_PCH_NAME Composer)

add_directories(. SRCS)

# Libraries to link with Composer
set(LIBRARIES Dreemchest)

# Inject the environment options to VisualStudio user configuration
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm512")
    set(USER_FILE ${TARGET_NAME}.vcxproj.user)
    set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${USER_FILE})
    configure_file(VisualStudio.user ${USER_FILE} @ONLY)
endif ()

# Setup the Qt library
if (DC_QT_SUPPORT MATCHES "Qt4")
    find_package(Qt4 REQUIRED QtGui QtOpenGL)
    set(LIBRARIES ${LIBRARIES} Qt4::QtGui Qt4::QtOpenGL)
    
elseif (DC_QT_SUPPORT MATCHES "Qt5")
    find_package(Qt5Widgets)
    find_package(Qt5OpenGL)
    set(LIBRARIES ${LIBRARIES} Qt5::Widgets Qt5::OpenGL)
    
endif ()

# Link with Relight
if (DC_WITH_RELIGHT)
    set(LIBRARIES ${LIBRARIES} Relight)
endif ()

# Find FBX SDK
find_package(FBX)

if (FBX_FOUND)
    set(LIBRARIES ${LIBRARIES} Fbx)
    add_definitions(-DHAVE_FBX)
endif()

# Find TIFF
find_package(TIFF)

if (TIFF_FOUND)
    set(LIBRARIES ${LIBRARIES} ${TIFF_LIBRARIES})
    include_directories(${TIFF_INCLUDE_DIR})
    add_definitions(-DHAVE_TIFF)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Setup the PCH
if (DC_USE_PCH)
    if (MSVC)
        set_source_files_properties(${DC_PCH_NAME}.cpp
            PROPERTIES
            COMPILE_FLAGS "/Yc${DC_PCH_NAME}.h"
            )
            
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FI${DC_PCH_NAME}.h")
    else()
        message(FATAL_ERROR "Precompiled headers are supported only for MSVC")
    endif()
endif()

# Create the Composer executable
add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${SRCS} Resources/Resources.qrc)
 
# Link dependencies
target_link_libraries(${TARGET_NAME} ${LIBRARIES})
