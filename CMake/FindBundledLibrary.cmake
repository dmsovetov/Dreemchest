macro(find_bundled_library NAME)
    find_package(Bundled${NAME})

    string(TOUPPER ${NAME} NAME_UPPER)

    set(FOUND_NAME "${NAME_UPPER}_FOUND")
    set(INCLUDE_DIR_NAME "${NAME_UPPER}_INCLUDE_DIR")
    set(LIBRARY_DIR_NAME "${NAME_UPPER}_LIBRARY_DIRS")
    set(LIBRARY_NAME "${NAME_UPPER}_LIBRARY")

    set(INCLUDE_DIR ${${INCLUDE_DIR_NAME}})
    set(LIBRARY_DIR ${${LIBRARY_DIR_NAME}})
    set(LIBRARY ${${LIBRARY_NAME}})
    set(FOUND ${${FOUND_NAME}})

    if (FOUND)
	    include_directories(${INCLUDE_DIR})
	    link_directories(${LIBRARY_DIR})
	    list(APPEND LIBRARIES ${LIBRARY})
	else()
	    message(STATUS "Bundled library ${NAME} not found")
    endif ()
endmacro()