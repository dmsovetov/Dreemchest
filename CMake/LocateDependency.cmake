macro(locate_dependency NAME)
    # Convert a dependency name to an uppercase
    string(TOUPPER ${NAME} NAME_UPPER)

    # Construct variable names
    set(BUNDLED_LIBRARY "BUNDLED_${NAME_UPPER}")
    set(SYSTEM_LIBRARY "SYSTEM_${NAME_UPPER}")

    # Locate dependency according to a requested location
    if (${${BUNDLED_LIBRARY}})
        message (STATUS "Looking for a bundled library ${NAME}")
        find_bundled_library(${NAME})
    elseif (${${SYSTEM_LIBRARY}})
        message (STATUS "Looking for a system library ${NAME}")
        find_package(${NAME})
    endif()
endmacro()