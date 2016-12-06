# Locate lua.h
find_path(LUA_INCLUDE_DIR
          PATHS ${PREFIX_PATH}
          NAMES lua.h
          PATH_SUFFIXES include
          NO_DEFAULT_PATH
          )

# Locate Lua library file
find_library(LUA_LIBRARY
             PATHS ${PREFIX_PATH}
             NAMES lua
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH
             )

# Set Lua version string
set(LUA_VERSION_STRING bundled)

# Complete module configuration
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(lua
                                  REQUIRED_VARS LUA_LIBRARY LUA_INCLUDE_DIR
                                  VERSION_VAR LUA_VERSION_STRING
                                  )