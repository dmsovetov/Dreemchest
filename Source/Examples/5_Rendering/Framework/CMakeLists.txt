file(GLOB SRCS ${dir} "*.cpp")
file(GLOB HEADERS ${dir} "*.h")

add_library(Framework ${SRCS} ${HEADERS})
target_link_libraries(Framework Dreemchest)
target_include_directories(Framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
source_group("Code" FILES ${SRCS} ${HEADERS})

set_property(TARGET Framework PROPERTY FOLDER "Examples")
