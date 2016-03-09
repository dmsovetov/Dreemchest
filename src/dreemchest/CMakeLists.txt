# Setup definitions
add_definitions(-DDC_BUILD_LIBRARY)

# Glob all source files
add_directories(Base BASE_SRCS)
add_directories(Assets ASSETS_SRCS)
add_directories(Ecs ECS_SRCS)
add_directories(Scene SCENE_SRCS)
add_directories(Network NETWORK_SRCS)
add_directories(Fx FX_SRCS)
add_directories(Introspection INTR_SRCS)

add_files(. ROOT_SRCS)
add_files(Mvvm MVVM_SRCS)
add_files(Io IO_SRCS)
add_files(Io/Serialization IO_SERIALIZATION_SRCS)
add_files(Io/Streams IO_STREAMS_SRCS)

# Locate the JSON library
find_package(Jsoncpp)

if (JSONCPP_INCLUDE_DIR AND JSONCPP_LIBRARY)
	add_definitions(-DHAVE_JSON)
	include_directories(${JSONCPP_INCLUDE_DIR})
endif (JSONCPP_INCLUDE_DIR AND JSONCPP_LIBRARY)

# Add Qt support to MVVM module
if (NOT (DC_QT_SUPPORT MATCHES "Disabled"))
	set(CMAKE_AUTOMOC ON)
	add_directories(Mvvm/Qt MVVM_QT_SRCS)
	set(MVVM_SRCS ${MVVM_SRCS} ${MVVM_QT_SRCS})

	if (DC_QT_SUPPORT MATCHES "Qt4")
		find_package(Qt4 REQUIRED QtGui)
		
	elseif (DC_QT_SUPPORT MATCHES "Qt5")
		find_package(Qt5Widgets)
		
	endif (DC_QT_SUPPORT MATCHES "Qt4")
	
endif (NOT (DC_QT_SUPPORT MATCHES "Disabled"))

# Threads module sources
if (DC_THREADS)
	add_files(Threads THREADS_SRCS)
	add_files(Threads/Task THREADS_TASK_SRCS)
	add_files(Threads/${DC_THREADS} THREADS_PLATFORM_SRCS)
endif(DC_THREADS)

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
	add_definitions(-DDC_OPENGL_ENABLED)
endif (DC_OPENGL_ENABLED)

# Sound module sources
if (DC_SOUND_ENABLED)
	find_package(OpenAL)
	find_package(OggVorbis)
	
	add_files(Sound SOUND_SRCS)
	add_files(Sound/Drivers SOUND_DRIVERS_SRCS)
	
	if (OPENAL_FOUND)
		set(OPENAL_ENABLED ON)
	endif ()
	
	if (OGGVORBIS_FOUND)
		set(OGGVORBIS_ENABLED ON)
	endif ()
	
	if (OPENAL_ENABLED)
		add_definitions(-DDC_HAVE_OPENAL)
		add_files(Sound/Drivers/OpenAL SOUND_OPENAL_SRCS)
		include_directories(${OPENAL_INCLUDE_DIR})
	endif (OPENAL_ENABLED)

	set(SOUND_DECODERS_SRCS
		Sound/Decoders/SoundDecoder.cpp
		Sound/Decoders/SoundDecoder.h
		Sound/Decoders/WavSoundDecoder.cpp
		Sound/Decoders/WavSoundDecoder.h)

	if (OGGVORBIS_ENABLED)
		add_definitions(-DDC_HAVE_VORBIS)
		set(SOUND_DECODERS_SRCS ${SOUND_DECODERS_SRCS} Sound/Decoders/OggSoundDecoder.cpp Sound/Decoders/OggSoundDecoder.h)
		include_directories(${OGGVORBIS_INCLUDE_DIR})
	endif ()

	source_group("Code\\Sound\\Decoders" FILES ${SOUND_DECODERS_SRCS})

	set (SOUND_SRCS ${SOUND_SRCS} ${SOUND_DRIVERS_SRCS} ${SOUND_DECODERS_SRCS} ${SOUND_OPENAL_SRCS})
endif (DC_SOUND_ENABLED)

# Setup the PCH
if (DC_USE_PCH)
	if (MSVC)
		set_source_files_properties(Dreemchest.cpp
			PROPERTIES
			COMPILE_FLAGS "/YcDreemchest.h"
			)
			
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FIDreemchest.h")
	else()
		message(FATAL_ERROR "Precompiled headers are supported only for MSVC")
	endif()
endif()

# Add the Dreemchest static library
add_library(Dreemchest STATIC ${ROOT_SRCS} ${BASE_SRCS} ${MVVM_SRCS} ${ECS_SRCS} ${IO_SRCS} ${IO_SERIALIZATION_SRCS} ${IO_STREAMS_SRCS} ${NETWORK_SRCS} ${SCENE_SRCS} ${THREADS_SRCS} ${THREADS_TASK_SRCS}  ${THREADS_PLATFORM_SRCS} ${PLATFORM_IMPL_SRCS} ${PLATFORM_SRCS} ${RENDERER_SRCS} ${FX_SRCS} ${SOUND_SRCS} ${ASSETS_SRCS} ${INTR_SRCS} )

# Link 3rd-party libraries
if (DC_BOX2D_ENABLED)
	target_link_libraries(Dreemchest Box2D)
endif (DC_BOX2D_ENABLED)

if (JSONCPP_LIBRARY)
	target_link_libraries(Dreemchest ${JSONCPP_LIBRARY})
endif (JSONCPP_LIBRARY)

if (OPENAL_LIBRARY)
	target_link_libraries(Dreemchest ${OPENAL_LIBRARY})
endif (OPENAL_LIBRARY)

if (OGGVORBIS_FOUND)
	target_link_libraries(Dreemchest ${OGGVORBIS_LIBRARIES})
endif (OGGVORBIS_FOUND)

if (WIN32)
	target_link_libraries(Dreemchest ws2_32)
endif (WIN32)

if (DC_OPENGL_ENABLED)
	target_link_libraries(Dreemchest ${OPENGL_gl_LIBRARY})
endif (DC_OPENGL_ENABLED)

if (DC_QT_SUPPORT MATCHES "Qt4")
	target_link_libraries(Dreemchest Qt4::QtGui)
endif (DC_QT_SUPPORT MATCHES "Qt4")

if (DC_QT_SUPPORT MATCHES "Qt5")
	target_link_libraries(Dreemchest Qt5::Widgets)
endif (DC_QT_SUPPORT MATCHES "Qt5")