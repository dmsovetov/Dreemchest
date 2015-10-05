# Setup variables
if (WIN32)
	set(DC_PLATFORM "Windows")
endif (WIN32)

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
file(GLOB PLATFORM_SRCS "Platform/*.cpp" "Platform/*.h" "Platform/${DC_PLATFORM}/*.cpp" "Platform/${DC_PLATFORM}/*.cpp")

# Renderer module sources
file(GLOB RENDERER_SRCS
	"Renderer/*.cpp"
	"Renderer/*.h"
)
	
if (DC_OPENGL_ENABLED)
	find_package(OpenGL REQUIRED)
	file(GLOB OPENGL_SRCS
		"Renderer/OpenGL/*.cpp"
		"Renderer/OpenGL/*.h"
		"Renderer/OpenGL/${DC_PLATFORM}/*.cpp"
		"Renderer/OpenGL/${DC_PLATFORM}/*.h"
	)
	set(RENDERER_SRCS ${RENDERER_SRCS} ${OPENGL_SRCS})
	add_definitions(-DDC_OPENGL_ENABLED)
endif (DC_OPENGL_ENABLED)

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

if (DC_OPENGL_ENABLED)
	target_link_libraries(Dreemchest ${OPENGL_gl_LIBRARY})
endif (DC_OPENGL_ENABLED)