set(ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/../../../Build/Examples/Assets)

add_executable(01_RendererInitialization WIN32 MACOSX_BUNDLE 01_RendererInitialization.cpp ${ASSETS})
target_link_libraries(01_RendererInitialization Dreemchest)

add_executable(02_FirstTriangle WIN32 MACOSX_BUNDLE 02_FirstTriangle.cpp ${ASSETS})
target_link_libraries(02_FirstTriangle Dreemchest)

add_executable(03_IndexBuffers WIN32 MACOSX_BUNDLE 03_IndexBuffers.cpp ${ASSETS})
target_link_libraries(03_IndexBuffers Dreemchest)

add_executable(04_Shaders WIN32 MACOSX_BUNDLE 04_Shaders.cpp ${ASSETS})
target_link_libraries(04_Shaders Dreemchest)

add_executable(05_ConstantBuffers WIN32 MACOSX_BUNDLE 05_ConstantBuffers.cpp ${ASSETS})
target_link_libraries(05_ConstantBuffers Dreemchest)

add_executable(06_FirstUbershader WIN32 MACOSX_BUNDLE 06_FirstUbershader.cpp ${ASSETS})
target_link_libraries(06_FirstUbershader Dreemchest)

add_executable(07_Cubes WIN32 MACOSX_BUNDLE 07_Cubes.cpp ${ASSETS})
target_link_libraries(07_Cubes Dreemchest)

add_executable(08_Textures WIN32 MACOSX_BUNDLE 08_Textures.cpp ${ASSETS})
target_link_libraries(08_Textures Dreemchest)

add_executable(09_Meshes WIN32 MACOSX_BUNDLE 09_Meshes.cpp ${ASSETS})
target_link_libraries(09_Meshes Dreemchest)

add_executable(10_StencilBuffer WIN32 MACOSX_BUNDLE 10_StencilBuffer.cpp ${ASSETS})
target_link_libraries(10_StencilBuffer Dreemchest)

add_executable(11_PointLights WIN32 MACOSX_BUNDLE 11_PointLights.cpp ${ASSETS})
target_link_libraries(11_PointLights Dreemchest)

add_executable(12_RenderingToTexture WIN32 MACOSX_BUNDLE 12_RenderingToTexture.cpp ${ASSETS})
target_link_libraries(12_RenderingToTexture Dreemchest)

add_executable(13_IBL WIN32 MACOSX_BUNDLE 13_IBL.cpp ${ASSETS})
target_link_libraries(13_IBL Dreemchest)

add_executable(14_RenderingToCubeMap WIN32 MACOSX_BUNDLE 14_RenderingToCubeMap.cpp ${ASSETS})
target_link_libraries(14_RenderingToCubeMap Dreemchest)

# Add solution folders
set_property(TARGET 01_RendererInitialization PROPERTY FOLDER "Examples")
set_property(TARGET 02_FirstTriangle PROPERTY FOLDER "Examples")
set_property(TARGET 03_IndexBuffers PROPERTY FOLDER "Examples")
set_property(TARGET 04_Shaders PROPERTY FOLDER "Examples")
set_property(TARGET 05_ConstantBuffers PROPERTY FOLDER "Examples")
set_property(TARGET 06_FirstUbershader PROPERTY FOLDER "Examples")
set_property(TARGET 07_Cubes PROPERTY FOLDER "Examples")
set_property(TARGET 08_Textures PROPERTY FOLDER "Examples")
set_property(TARGET 09_Meshes PROPERTY FOLDER "Examples")
set_property(TARGET 10_StencilBuffer PROPERTY FOLDER "Examples")
set_property(TARGET 11_PointLights PROPERTY FOLDER "Examples")
set_property(TARGET 12_RenderingToTexture PROPERTY FOLDER "Examples")
set_property(TARGET 13_IBL PROPERTY FOLDER "Examples")
set_property(TARGET 14_RenderingToCubeMap PROPERTY FOLDER "Examples")

if (APPLE)
    set_source_files_properties(${ASSETS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()
