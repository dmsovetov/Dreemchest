# Locate png.h
find_path(PNG_INCLUDE_DIR
          PATHS ${PREFIX_PATH}
          NAMES png.h
          PATH_SUFFIXES include
          NO_DEFAULT_PATH
          )

# Locate libpng library file
find_library(PNG_LIBRARY
             PATHS ${PREFIX_PATH}
             NAMES png libpng
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )

# Set libpng version string
set(PNG_VERSION_STRING bundled)

# Complete module configuration
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(png
                                  REQUIRED_VARS PNG_LIBRARY PNG_INCLUDE_DIR
                                  VERSION_VAR PNG_VERSION_STRING
                                  )