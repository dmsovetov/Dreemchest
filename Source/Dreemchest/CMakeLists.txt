# Setup definitions
add_definitions(-DDC_BUILD_LIBRARY)

# Set the precompiled header name
set(DC_PCH_NAME Dreemchest)

# Glob all source files
add_directories(Base BASE_SRCS)
add_directories(Assets ASSETS_SRCS)
add_directories(Ecs ECS_SRCS)
add_directories(Scene SCENE_SRCS)
add_directories(Network NETWORK_SRCS)
add_directories(Fx FX_SRCS)
add_directories(Reflection REFLECTION_SRCS)

add_files(. ROOT_SRCS)
add_files(Io IO_SRCS)
add_files(Io/Serialization IO_SERIALIZATION_SRCS)
add_files(Io/Streams IO_STREAMS_SRCS)

# Locate the Box2D library
if (BUNDLED_BOX2D)
    find_package(Box2D)

    if (BOX2D_INCLUDE_DIR AND BOX2D_LIBRARY)
	    include_directories(${BOX2D_INCLUDE_DIR})
    endif ()
endif()

# Locate the Google Test library
if (BUNDLED_GTEST)
    find_package(GTest)

    if (GTEST_INCLUDE_DIRS AND GTEST_BOTH_LIBRARIES)
	    include_directories(${GTEST_INCLUDE_DIRS})
    endif ()
endif()

# Locate the JSON library
if (BUNDLED_JSONCPP)
    find_package(Jsoncpp)

    if (JSONCPP_INCLUDE_DIR AND JSONCPP_LIBRARY)
	    include_directories(${JSONCPP_INCLUDE_DIR})
    endif ()
endif()

# Locate the libpng library
if (BUNDLED_LIBPNG)
    find_package(PNG)

    if (PNG_INCLUDE_DIR AND PNG_LIBRARY)
	    include_directories(${PNG_INCLUDE_DIR})
    endif ()
endif()

# Locate the libtiff library
if (BUNDLED_LIBTIFF)
    find_package(TIFF)

    if (TIFF_INCLUDE_DIR AND TIFF_LIBRARY)
	    include_directories(${TIFF_INCLUDE_DIR})
    endif ()
endif()

# Locate the Lua library
if (BUNDLED_LUA)
    find_package(Lua)

    if (LUA_INCLUDE_DIR AND LUA_LIBRARY)
	    include_directories(${LUA_INCLUDE_DIR})
    endif ()
endif()

# Locate the Ogg and Vorbis libraries
if (BUNDLED_OGGVORBIS)
    find_package(OggVorbis)

    if (OGGVORBIS_INCLUDE_DIR AND OGGVORBIS_LIBRARY)
	    include_directories(${OGGVORBIS_INCLUDE_DIR})
    endif ()
endif()

# Locate the OpenAL library
if (BUNDLED_OPENAL)
    find_package(OpenAL)

    if (OPENAL_INCLUDE_DIR AND OPENAL_LIBRARY)
	    include_directories(${OPENAL_INCLUDE_DIR})
    endif ()
endif()

# Locate the zlib library
if (BUNDLED_ZLIB)
    find_package(Zlib)

    if (ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
	    include_directories(${ZLIB_INCLUDE_DIR})
    endif ()
endif()

# Locate the cURL library
if (BUNDLED_CURL)
    find_package(Curl)

    if (CURL_INCLUDE_DIR AND CURL_LIBRARY)
	    include_directories(${CURL_INCLUDE_DIR})
    endif ()
endif()

# Setup Qt support
if (NOT (DC_QT_SUPPORT MATCHES "Disabled"))
	set(CMAKE_AUTOMOC ON)

	if (DC_QT_SUPPORT MATCHES "Qt4")
		find_package(Qt4 REQUIRED QtGui)
		
	elseif (DC_QT_SUPPORT MATCHES "Qt5")
		find_package(Qt5Widgets)
		
	endif ()
	
endif ()

# Threads module sources
if (DC_THREADS)
	add_files(Threads THREADS_SRCS)
	add_files(Threads/Task THREADS_TASK_SRCS)
	add_files(Threads/${DC_THREADS} THREADS_PLATFORM_SRCS)
endif()

# Platform module sources
add_files(Platform PLATFORM_SRCS)
add_files(Platform/${DC_PLATFORM} PLATFORM_IMPL_SRCS)

# Renderer module sources
add_files(Renderer RENDERER_SRCS)

if (DC_OPENGL_ENABLED)
	find_package(OpenGL REQUIRED)
	add_files(Renderer/OpenGL OPENGL_SRCS)
	add_files(Renderer/OpenGL/${DC_PLATFORM} OPENGL_PLATFORM_SRCS)
	set(RENDERER_SRCS ${RENDERER_SRCS} ${OPENGL_SRCS} ${OPENGL_PLATFORM_SRCS})
endif ()

# Sound module sources
if (DC_SOUND_ENABLED)
	add_files(Sound SOUND_SRCS)
	add_files(Sound/Drivers SOUND_DRIVERS_SRCS)
	
	if (OPENAL_FOUND)
		add_files(Sound/Drivers/OpenAL SOUND_OPENAL_SRCS)
	endif ()

	set(SOUND_DECODERS_SRCS
		Sound/Decoders/SoundDecoder.cpp
		Sound/Decoders/SoundDecoder.h
		Sound/Decoders/WavSoundDecoder.cpp
		Sound/Decoders/WavSoundDecoder.h)

	if (OGGVORBIS_ENABLED)
		set(SOUND_DECODERS_SRCS ${SOUND_DECODERS_SRCS} Sound/Decoders/OggSoundDecoder.cpp Sound/Decoders/OggSoundDecoder.h)
	endif ()

	source_group("Code\\Sound\\Decoders" FILES ${SOUND_DECODERS_SRCS})

	set (SOUND_SRCS ${SOUND_SRCS} ${SOUND_DRIVERS_SRCS} ${SOUND_DECODERS_SRCS} ${SOUND_OPENAL_SRCS})
endif ()

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

# Add the Dreemchest static library
add_library(Dreemchest STATIC ${ROOT_SRCS} ${BASE_SRCS} ${ECS_SRCS} ${IO_SRCS} ${IO_SERIALIZATION_SRCS} ${IO_STREAMS_SRCS} ${NETWORK_SRCS} ${SCENE_SRCS} ${THREADS_SRCS} ${THREADS_TASK_SRCS}  ${THREADS_PLATFORM_SRCS} ${PLATFORM_IMPL_SRCS} ${PLATFORM_SRCS} ${RENDERER_SRCS} ${FX_SRCS} ${SOUND_SRCS} ${ASSETS_SRCS} ${REFLECTION_SRCS} )

# Link 3rd-party libraries
if (BOX2D_FOUND)
	target_link_libraries(Dreemchest Box2D)
endif ()

if (JSONCPP_FOUND)
	target_link_libraries(Dreemchest ${JSONCPP_LIBRARY})
endif ()

if (OPENAL_FOUND)
	target_link_libraries(Dreemchest ${OPENAL_LIBRARY})
endif ()

if (OGGVORBIS_FOUND)
	target_link_libraries(Dreemchest ${OGGVORBIS_LIBRARIES})
endif ()

if (WIN32)
	target_link_libraries(Dreemchest ws2_32)
endif ()

if (DC_OPENGL_ENABLED)
	target_link_libraries(Dreemchest ${OPENGL_gl_LIBRARY})
endif ()

if (DC_QT_SUPPORT MATCHES "Qt4")
	target_link_libraries(Dreemchest Qt4::QtGui)
endif ()

if (DC_QT_SUPPORT MATCHES "Qt5")
	target_link_libraries(Dreemchest Qt5::Widgets)
endif ()

# MacOS frameworks
if (DC_PLATFORM MATCHES "MacOS")
    target_link_libraries(Dreemchest "-framework AppKit")

    if (DC_OPENGL_ENABLED)
        target_link_libraries(Dreemchest "-framework CoreVideo")
    endif ()
endif ()

# Generate configuration file
configure_file(BuildConfig.in.h BuildConfig.h)
target_include_directories(Dreemchest PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Generate a configuration hash constant
file(MD5 ${CMAKE_CURRENT_BINARY_DIR}/BuildConfig.h DREEMCHEST_VERSION)
configure_file(BuildConfig.in.h BuildConfig.h)