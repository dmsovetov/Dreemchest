# Locate vorbis/codec.h
find_path(VORBIS_INCLUDE_DIR
          PATHS ${PREFIX_PATH}
          NAMES vorbis/codec.h
          PATH_SUFFIXES include
          NO_DEFAULT_PATH
          )

# Locate vorbis library file
find_library(VORBIS_LIBRARY
             PATHS ${PREFIX_PATH}
             NAMES vorbis libvorbis
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )

# Set vorbis version string
set(VORBIS_VERSION_STRING bundled)

# Complete module configuration
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(vorbis
                                  REQUIRED_VARS VORBIS_LIBRARY VORBIS_INCLUDE_DIR
                                  VERSION_VAR VORBIS_VERSION_STRING
                                  )