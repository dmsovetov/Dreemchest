find_package(GTest REQUIRED)

add_files(. SRCS)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(Tests ${SRCS})
target_link_libraries(Tests ${GTEST_BOTH_LIBRARIES} Dreemchest)