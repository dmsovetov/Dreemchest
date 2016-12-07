add_executable(01_HelloWorld WIN32 MACOSX_BUNDLE 01_HelloWorld.cpp)
target_link_libraries(01_HelloWorld Dreemchest)

add_executable(02_CreatingWindows WIN32 MACOSX_BUNDLE 02_CreatingWindows.cpp)
target_link_libraries(02_CreatingWindows Dreemchest)

add_executable(03_WindowEvents WIN32 MACOSX_BUNDLE 03_WindowEvents.cpp)
target_link_libraries(03_WindowEvents Dreemchest)

add_executable(04_RendererInitialization WIN32 MACOSX_BUNDLE 04_RendererInitialization.cpp)
target_link_libraries(04_RendererInitialization Dreemchest)

add_executable(05_Files WIN32 MACOSX_BUNDLE 05_Files.cpp)
target_link_libraries(05_Files Dreemchest)

add_executable(06_BasicNetworking WIN32 MACOSX_BUNDLE 06_BasicNetworking.cpp)
target_link_libraries(06_BasicNetworking Dreemchest)

add_executable(07_TCPSockets WIN32 MACOSX_BUNDLE 07_TCPSockets.cpp)
target_link_libraries(07_TCPSockets Dreemchest)

add_executable(08_NetworkHandlers WIN32 MACOSX_BUNDLE 08_NetworkHandlers.cpp)
target_link_libraries(08_NetworkHandlers Dreemchest)

#add_executable(09_ModelViewViewModel WIN32 MACOSX_BUNDLE 09_ModelViewViewModel.cpp)
#target_link_libraries(09_ModelViewViewModel Dreemchest)

if (DC_QT_ENABLED)
#	add_executable(10_MvvmQt 10_MvvmQt.cpp)
#	target_link_libraries(10_MvvmQt Dreemchest)
endif (DC_QT_ENABLED)

add_executable(11_Particles WIN32 MACOSX_BUNDLE 11_Particles.cpp)
target_link_libraries(11_Particles Dreemchest)

if (DC_SOUND_ENABLED)
	add_executable(12_Sound WIN32 MACOSX_BUNDLE 12_Sound.cpp)
	target_link_libraries(12_Sound Dreemchest)
endif (DC_SOUND_ENABLED)

add_executable(13_Scene WIN32 MACOSX_BUNDLE 13_Scene.cpp)
target_link_libraries(13_Scene Dreemchest)

# Add solution folders
set_property(TARGET 01_HelloWorld PROPERTY FOLDER "Examples")
set_property(TARGET 02_CreatingWindows PROPERTY FOLDER "Examples")
set_property(TARGET 03_WindowEvents PROPERTY FOLDER "Examples")
set_property(TARGET 04_RendererInitialization PROPERTY FOLDER "Examples")
set_property(TARGET 05_Files PROPERTY FOLDER "Examples")
set_property(TARGET 06_BasicNetworking PROPERTY FOLDER "Examples")
set_property(TARGET 07_TCPSockets PROPERTY FOLDER "Examples")
set_property(TARGET 08_NetworkHandlers PROPERTY FOLDER "Examples")
set_property(TARGET 11_Particles PROPERTY FOLDER "Examples")

if (DC_SOUND_ENABLED)
	set_property(TARGET 12_Sound PROPERTY FOLDER "Examples")
endif()
set_property(TARGET 13_Scene PROPERTY FOLDER "Examples")