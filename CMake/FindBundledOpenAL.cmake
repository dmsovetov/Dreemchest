# Locate al.h
find_path(OPENAL_INCLUDE_DIR
          PATHS ${PREFIX_PATH}
          NAMES al.h
          PATH_SUFFIXES include/AL include/OpenAL include
          NO_DEFAULT_PATH
          )

# Locate OpenAL library file
find_library(OPENAL_LIBRARY
             PATHS ${PREFIX_PATH}
             NAMES OpenAL al openal OpenAL32
             PATH_SUFFIXES lib64 lib libs64 libs ${_OpenAL_ARCH_DIR}
             NO_DEFAULT_PATH
             )

# Set OpenAL version string
set(OPENAL_VERSION_STRING bundled)

# Complete module configuration
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenAL
                                  REQUIRED_VARS OPENAL_LIBRARY OPENAL_INCLUDE_DIR
                                  VERSION_VAR OPENAL_VERSION_STRING
                                  )