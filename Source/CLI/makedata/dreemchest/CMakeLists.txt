project(dreemchest)

if (APPLE)
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

find_package(PythonLibs REQUIRED)
add_library(dreemchest SHARED dreemchest.cpp)
target_include_directories(dreemchest PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(dreemchest PRIVATE ${PYTHON_LIBRARIES})