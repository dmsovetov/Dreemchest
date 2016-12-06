# Locate json.h
find_path(JSONCPP_INCLUDE_DIR
          PATHS ${PREFIX_PATH}
          NAMES jsoncpp/json/json.h json/json.h
          PATH_SUFFIXES include
          NO_DEFAULT_PATH
          )

# Locate jsoncpp library file
find_library(JSONCPP_LIBRARY
             PATHS ${PREFIX_PATH}
             NAMES jsoncpp
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )

# Set jsoncpp version string
set(JSONCPP_VERSION_STRING bundled)

# Complete module configuration
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(jsoncpp
                                  REQUIRED_VARS JSONCPP_LIBRARY JSONCPP_INCLUDE_DIR
                                  VERSION_VAR JSONCPP_VERSION_STRING
                                  )