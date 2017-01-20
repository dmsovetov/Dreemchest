file(GLOB SRCS ${dir} "*.cpp")
file(GLOB HEADERS ${dir} "*.h")

add_library(Framework ${SRCS} ${HEADERS})
source_group("Code" FILES ${SRCS} ${HEADERS})

set_property(TARGET Framework PROPERTY FOLDER "Examples")