# This path should be added to all targets as a source
set(ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/../../../Build/Examples/Assets)

add_subdirectory(1_Basic)
add_subdirectory(2_Advanced)
add_subdirectory(3_Lighting)
add_subdirectory(4_PBR)

if (APPLE)
    set_source_files_properties(${ASSETS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()
