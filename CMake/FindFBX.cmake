set(FBX_LIBNAME "libfbxsdk-md")
set(FBX_ARCH x86)

if (CMAKE_GENERATOR MATCHES "Visual Studio 12 2013")
	set(FBX_LIBDIR "vs2013")
endif()

# Set the FXB search paths
set(FBX_SEARCH_PATHS $ENV{FBX_DIR})

# Search for headers & debug/release libraries
find_path(FBX_INCLUDE_DIR "fbxsdk.h"
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "include")
	
find_library(FBX_LIBRARY_RELEASE ${FBX_LIBNAME}
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "lib/${FBX_LIBDIR}/${FBX_ARCH}/release")
	
find_library(FBX_LIBRARY_DEBUG ${FBX_LIBNAME}
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "lib/${FBX_LIBDIR}/${FBX_ARCH}/debug")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FbxSdk DEFAULT_MSG FBX_LIBNAME FBX_INCLUDE_DIR)

if (FBX_INCLUDE_DIR AND FBX_LIBRARY_DEBUG AND FBX_LIBRARY_RELEASE)
	if(NOT FBX_FOUND)
		add_library(Fbx STATIC IMPORTED)
		set_property(TARGET Fbx PROPERTY IMPORTED_LOCATION_RELEASE ${FBX_LIBRARY_RELEASE})
		set_property(TARGET Fbx PROPERTY IMPORTED_LOCATION_DEBUG   ${FBX_LIBRARY_DEBUG})
		set_property(TARGET Fbx APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${FBX_INCLUDE_DIR})

		set(FBX_FOUND YES)
	endif()
else ()
	set(FBX_FOUND NO)
endif()