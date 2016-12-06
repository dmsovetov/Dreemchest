# Locate Box2D/Box2D.h
find_path(BOX2D_INCLUDE_DIR
          PATHS ${PREFIX_PATH}
          NAMES Box2D/Box2D.h
          PATH_SUFFIXES include
          NO_DEFAULT_PATH
          )

# Locate Box2D library file
find_library(BOX2D_LIBRARY
             PATHS ${CMAKE_PREFIX_PATH}
             NAMES box2d Box2d BOX2D Box2D
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )

# Set BOX2D version string
set(BOX2D_VERSION_STRING bundled)

# Complete module configuration
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Box2D
                                  REQUIRED_VARS BOX2D_LIBRARY BOX2D_INCLUDE_DIR
                                  VERSION_VAR BOX2D_VERSION_STRING
                                  )