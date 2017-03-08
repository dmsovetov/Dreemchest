cmake_minimum_required(VERSION 3.2)

project(dreemchest)

find_path (DREEMCHEST_INCLUDE_DIR Dreemchest.h PATH_SUFFIXES include/Dreemchest)
find_path (NIMBLE_INCLUDE_DIR Nimble.h PATH_SUFFIXES include/Nimble)
find_library (DREEMCHEST_LIBRARY Dreemchest PATH_SUFFIXES lib)

if(NOT DREEMCHEST_INCLUDE_DIR)
	message(FATAL_ERROR "Dreemchest library include file not found")
endif()

if(NOT NIMBLE_INCLUDE_DIR)
	message(FATAL_ERROR "Nimble library include file not found")
endif()

if(NOT DREEMCHEST_LIBRARY)
	message(FATAL_ERROR "Dreemchest library file not found")
endif()

if (APPLE)
	set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
else()
	set(CMAKE_SHARED_LIBRARY_SUFFIX ".pyd")
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

find_package(PythonLibs REQUIRED)
add_library(dreemchest SHARED dreemchest.cpp)
target_include_directories(dreemchest PRIVATE ${PYTHON_INCLUDE_DIRS} ${DREEMCHEST_INCLUDE_DIR} ${NIMBLE_INCLUDE_DIR})
target_link_libraries(dreemchest PRIVATE ${PYTHON_LIBRARIES} ${DREEMCHEST_LIBRARY})

install(TARGETS dreemchest DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})