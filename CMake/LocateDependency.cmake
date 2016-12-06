macro(locate_dependency NAME)
    # Convert a dependency name to an uppercase
    string(TOUPPER ${NAME} NAME_UPPER)

    # Construct variable names
    set(BUNDLED_LIBRARY "BUNDLED_${NAME_UPPER}")
    set(SYSTEM_LIBRARY "SYSTEM_${NAME_UPPER}")
    set(LIBRARY_NAME "${NAME_UPPER}_LIBRARY")
    set(FOUND_NAME "${NAME_UPPER}_FOUND")

    # Locate dependency according to a requested location
    if (${${BUNDLED_LIBRARY}})
        find_bundled_library(${NAME})
    elseif (${${SYSTEM_LIBRARY}})
        find_package(${NAME})
    endif()

    # Push a dependency name to a list
    if (${${FOUND_NAME}})
	    list(APPEND DEPENDENCIES ${${LIBRARY_NAME}})
    endif ()
endmacro()