add_executable(01_RendererInitialization WIN32 MACOSX_BUNDLE 01_RendererInitialization.cpp)
target_link_libraries(01_RendererInitialization Dreemchest)

add_executable(02_FirstTriangle WIN32 MACOSX_BUNDLE 02_FirstTriangle.cpp)
target_link_libraries(02_FirstTriangle Dreemchest)

add_executable(03_IndexBuffers WIN32 MACOSX_BUNDLE 03_IndexBuffers.cpp)
target_link_libraries(03_IndexBuffers Dreemchest)

add_executable(04_Shaders WIN32 MACOSX_BUNDLE 04_Shaders.cpp)
target_link_libraries(04_Shaders Dreemchest)

add_executable(05_FeatureLayouts WIN32 MACOSX_BUNDLE 05_FeatureLayouts.cpp)
target_link_libraries(05_FeatureLayouts Dreemchest)

# Add solution folders
set_property(TARGET 01_RendererInitialization PROPERTY FOLDER "Examples")
set_property(TARGET 02_FirstTriangle PROPERTY FOLDER "Examples")
set_property(TARGET 03_IndexBuffers PROPERTY FOLDER "Examples")
set_property(TARGET 04_Shaders PROPERTY FOLDER "Examples")
set_property(TARGET 05_FeatureLayouts PROPERTY FOLDER "Examples")
