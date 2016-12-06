# Locate gtest/gtest.h
find_path(GTEST_INCLUDE_DIR
          PATHS ${PREFIX_PATH}
          NAMES gtest/gtest.h
          PATH_SUFFIXES include
          NO_DEFAULT_PATH
          )

# Locate gtest libraries
find_library(GTEST_LIBRARY
             PATHS ${PREFIX_PATH}
             NAMES gtest
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )
find_library(GTEST_LIBRARY_MAIN
             PATHS ${PREFIX_PATH}
             NAMES gmock_main
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )
find_library(GMOCK_LIBRARY
             PATHS ${PREFIX_PATH}
             NAMES gmock
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )
find_library(GMOCK_LIBRARY_MAIN
             PATHS ${PREFIX_PATH}
             NAMES gtest_main
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )

# Set GTest version string
set(GTEST_VERSION_STRING bundled)

# Compose a list of required libraries
set(GTEST_LIBRARIES GTEST_LIBRARY GTEST_LIBRARY_MAIN GMOCK_LIBRARY GMOCK_LIBRARY_MAIN)

# Complete module configuration
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GTest
                                  REQUIRED_VARS GTEST_LIBRARY GTEST_LIBRARY_MAIN GMOCK_LIBRARY GMOCK_LIBRARY_MAIN GTEST_INCLUDE_DIR
                                  VERSION_VAR GTEST_VERSION_STRING
                                  )