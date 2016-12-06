# Locate zlib.h
find_path(ZLIB_INCLUDE_DIR
          PATHS ${PREFIX_PATH}
          NAMES zlib.h
          PATH_SUFFIXES include
          NO_DEFAULT_PATH
          )

# Locate zlib library file
find_library(ZLIB_LIBRARY
             PATHS ${PREFIX_PATH}
             NAMES z zlib zdll zlib1
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )

# Set zlib version string
set(ZLIB_VERSION_STRING bundled)

# Complete module configuration
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(zlib
                                  REQUIRED_VARS ZLIB_LIBRARY ZLIB_INCLUDE_DIR
                                  VERSION_VAR ZLIB_VERSION_STRING
                                  )