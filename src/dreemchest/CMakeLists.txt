# Setup definitions
add_definitions(-DDC_BUILD_LIBRARY)

# Glob all source files
add_directories(Base BASE_SRCS)
add_directories(Ecs ECS_SRCS)
add_directories(Scene SCENE_SRCS)
add_directories(Network NETWORK_SRCS)

add_files(. ROOT_SRCS)
add_files(Mvvm MVVM_SRCS)
add_files(Io IO_SRCS)
add_files(Io/Serialization IO_SERIALIZATION_SRCS)
add_files(Io/Streams IO_STREAMS_SRCS)

if (DC_QT_ENABLED)
	add_files(Mvvm/Qt MVVM_QT_SRCS)
	set(MVVM_SRCS ${MVVM_SRCS} ${MVVM_QT_SRCS})
	find_package(Qt4 REQUIRED QtGui)
endif (DC_QT_ENABLED)

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

# Add the Dreemchest static library
add_library(Dreemchest STATIC ${ROOT_SRCS} ${BASE_SRCS} ${MVVM_SRCS} ${ECS_SRCS} ${IO_SRCS} ${IO_SERIALIZATION_SRCS} ${IO_STREAMS_SRCS} ${NETWORK_SRCS} ${SCENE_SRCS} ${THREADS_SRCS} ${THREADS_TASK_SRCS}  ${THREADS_PLATFORM_SRCS} ${PLATFORM_IMPL_SRCS} ${PLATFORM_SRCS} ${RENDERER_SRCS})

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

if (DC_QT_ENABLED)
	target_link_libraries(Dreemchest Qt4::QtGui)
endif (DC_QT_ENABLED)