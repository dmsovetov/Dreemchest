# Locate tiff.h
find_path(TIFF_INCLUDE_DIR
          PATHS ${PREFIX_PATH}
          NAMES tiff.h
          PATH_SUFFIXES include
          NO_DEFAULT_PATH
          )

# Locate TIFF library file
find_library(TIFF_LIBRARY
             PATHS ${PREFIX_PATH}
             NAMES tiff libtiff tiff3 libtiff3
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )

# Set TIFF version string
set(TIFF_VERSION_STRING bundled)

# Complete module configuration
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(tiff
                                  REQUIRED_VARS TIFF_LIBRARY TIFF_INCLUDE_DIR
                                  VERSION_VAR TIFF_VERSION_STRING
                                  )