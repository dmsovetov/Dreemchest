# Locate curl/curl.h
find_path(CURL_INCLUDE_DIR
          PATHS ${PREFIX_PATH}
          NAMES curl/curl.h
          PATH_SUFFIXES include
          NO_DEFAULT_PATH
          )

# Locate cURL library file
find_library(CURL_LIBRARY
             PATHS ${CMAKE_PREFIX_PATH}
             NAMES curl curllib libcurl_imp curllib_static libcurl
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )

# Set zlib version string
set(CURL_VERSION_STRING bundled)

# Complete module configuration
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cURL
                                  REQUIRED_VARS CURL_LIBRARY CURL_INCLUDE_DIR
                                  VERSION_VAR CURL_VERSION_STRING
                                  )