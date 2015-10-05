# Glob all source files
file(GLOB_RECURSE BASE_SRCS "Base/*.cpp" "Base/*.h")
file(GLOB_RECURSE ECS_SRCS "Ecs/*.cpp" "Ecs/*.h")
file(GLOB_RECURSE NETWORK_SRCS "Network/*.cpp" "Network/*.h")
file(GLOB_RECURSE SCENE_SRCS "Scene/*.cpp" "Scene/*.h")

# I/O module sources
file(GLOB IO_SRCS "Io/*.cpp" "Io/*.h" "Io/serialization/*.cpp" "Io/serialization/*.h" "Io/streams/*.cpp" "Io/streams/*.h")

# Threads module sources
file(GLOB THREADS_SRCS "Threads/*.cpp" "Threads/*.h")

if (WIN32)
	file(GLOB PLAT_THREADS_SRC "Threads/Windows/*.cpp" "Threads/Windows/*.h" )
	add_definitions(-DDC_THREADS_WINDOWS)
endif(WIN32)

# Platform module sources
file(GLOB PLATFORM_SRCS "Platform/*.cpp" "Platform/*.h")

if (WIN32)
	file(GLOB PLAT_PLATFORM_SRC "Platform/Windows/*.cpp" "Platform/Windows/*.h" )
	add_definitions(-DDC_PLATFORM_WINDOWS)	
endif (WIN32)

file(GLOB 		  RENDERER_SRCS "Renderer/*.cpp" "Renderer/*.h")

# Add definitions
add_definitions(-DDC_BUILD_LIBRARY)

if (WIN32)
	add_definitions(-DDC_PLATFORM_WINDOWS)
endif (WIN32)

# Add the Dreemchest static library
add_library(Dreemchest STATIC ${BASE_SRCS} ${ECS_SRCS} ${IO_SRCS} ${NETWORK_SRCS} ${SCENE_SRCS} ${THREADS_SRCS} ${PLAT_THREADS_SRC} ${PLAT_PLATFORM_SRC} ${PLATFORM_SRCS} ${RENDERER_SRCS})

# Link 3rd-party libraries
if (DC_BOX2D_ENABLED)
	target_link_libraries(Dreemchest Box2D)
endif (DC_BOX2D_ENABLED)

if (WIN32)
	target_link_libraries(Dreemchest ws2_32)
endif (WIN32)