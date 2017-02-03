# Setup definitions
add_definitions(-DDC_BUILD_LIBRARY)

# Set the precompiled header name
set(DC_PCH_NAME Dreemchest)

# Locate all dependencies
find_package(OpenAL)
find_package(Zlib)
find_package(cURL)
find_package(Lua)
find_package(TIFF)
find_package(PNG)
find_package(JsonCpp)
find_package(GTest)
find_package(Box2D)
find_package(Ogg)
find_package(Vorbis)

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

if (ZLIB_FOUND)
    set(IO_STREAMS_SRCS ${IO_STREAMS_SRCS}
        Io/processors/ZLibBufferCompressor.cpp
        Io/processors/ZLibBufferCompressor
        Io/processors/IBufferCompressor.h)
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
add_files(Renderer/Commands RENDERER_COMMANDS_SRCS)

if (DC_OPENGL_ENABLED)
    if (DC_PLATFORM MATCHES "iOS")
        find_package(OpenGLES2 REQUIRED)
        set(OPENGL_gl_LIBRARY ${OPENGLES2_LIBRARIES})
    else ()
        find_package(OpenGL REQUIRED)
    endif ()
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

    if (OGGVORBIS_FOUND)
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
add_library(Dreemchest STATIC
    ${ROOT_SRCS}
    ${BASE_SRCS}
    ${ECS_SRCS}
    ${IO_SRCS}
    ${IO_SERIALIZATION_SRCS}
    ${IO_STREAMS_SRCS} 
    ${NETWORK_SRCS}
    ${SCENE_SRCS}
    ${THREADS_SRCS}
    ${THREADS_TASK_SRCS}
    ${THREADS_PLATFORM_SRCS}
    ${PLATFORM_IMPL_SRCS}
    ${PLATFORM_SRCS}
    ${RENDERER_SRCS}
    ${RENDERER_COMMANDS_SRCS}
    ${FX_SRCS}
    ${SOUND_SRCS}
    ${ASSETS_SRCS}
    ${REFLECTION_SRCS}
    )

if (WIN32)
    target_link_libraries(Dreemchest ws2_32)
endif ()

if (DC_OPENGL_ENABLED)
    target_link_libraries(Dreemchest ${OPENGL_gl_LIBRARY})
endif ()

# MacOS frameworks
if (DC_PLATFORM MATCHES "MacOS")
    target_link_libraries(Dreemchest "-framework AppKit")

    if (DC_OPENGL_ENABLED)
        target_link_libraries(Dreemchest "-framework CoreVideo")
    endif ()
elseif (DC_PLATFORM MATCHES "iOS")
    target_link_libraries(Dreemchest "-framework UIKit -framework Foundation")

    if (DC_OPENGL_ENABLED)
        target_link_libraries(Dreemchest "-framework QuartzCore")
    endif ()
endif ()

# Generate configuration file
configure_file(BuildConfig.in.h BuildConfig.h)
target_include_directories(Dreemchest PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Generate a configuration hash constant
file(MD5 ${CMAKE_CURRENT_BINARY_DIR}/BuildConfig.h DREEMCHEST_VERSION)
configure_file(BuildConfig.in.h BuildConfig.h)

# Link all requested libraries
if (BOX2D_FOUND)
    target_link_libraries(Dreemchest ${BOX2D_LIBRARY})
    target_include_directories(Dreemchest PRIVATE ${BOX2D_INCLUDE_DIR})
endif ()

if (JSONCPP_FOUND)
    target_link_libraries(Dreemchest ${JSONCPP_LIBRARY})
    target_include_directories(Dreemchest PUBLIC ${JSONCPP_INCLUDE_DIR})
endif ()

if (ZLIB_FOUND)
    target_link_libraries(Dreemchest ${ZLIB_LIBRARY})
    target_include_directories(Dreemchest PRIVATE ${ZLIB_INCLUDE_DIR})
endif ()

if (OGG_FOUND)
    target_link_libraries(Dreemchest ${OGG_LIBRARY})
    target_include_directories(Dreemchest PRIVATE ${OGG_INCLUDE_DIR})
endif ()

if (VORBIS_FOUND)
    target_link_libraries(Dreemchest ${VORBIS_LIBRARY})
    target_include_directories(Dreemchest PRIVATE ${VORBIS_INCLUDE_DIR})
endif ()

if (OPENAL_FOUND)
    target_link_libraries(Dreemchest ${OPENAL_LIBRARY})
    target_include_directories(Dreemchest PRIVATE ${OPENAL_INCLUDE_DIR})
endif ()