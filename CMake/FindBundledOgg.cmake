# Locate ogg/ogg.h
find_path(OGG_INCLUDE_DIR
          PATHS ${PREFIX_PATH}
          NAMES ogg/ogg.h
          PATH_SUFFIXES include
          NO_DEFAULT_PATH
          )

# Locate ogg library file
find_library(OGG_LIBRARY
             PATHS ${PREFIX_PATH}
             NAMES ogg libogg
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )

# Set ogg version string
set(OGG_VERSION_STRING bundled)

# Complete module configuration
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ogg
                                  REQUIRED_VARS OGG_LIBRARY OGG_INCLUDE_DIR
                                  VERSION_VAR OGG_VERSION_STRING
                                  )