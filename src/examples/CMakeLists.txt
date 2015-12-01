add_executable(01_HelloWorld 01_HelloWorld.cpp)
target_link_libraries(01_HelloWorld Dreemchest)

add_executable(02_CreatingWindows 02_CreatingWindows.cpp)
target_link_libraries(02_CreatingWindows Dreemchest)

add_executable(03_WindowEvents 03_WindowEvents.cpp)
target_link_libraries(03_WindowEvents Dreemchest)

add_executable(04_RendererInitialization 04_RendererInitialization.cpp)
target_link_libraries(04_RendererInitialization Dreemchest)

add_executable(05_Files 05_Files.cpp)
target_link_libraries(05_Files Dreemchest)

add_executable(06_BasicNetworking 06_BasicNetworking.cpp)
target_link_libraries(06_BasicNetworking Dreemchest)

add_executable(07_TCPSockets 07_TCPSockets.cpp)
target_link_libraries(07_TCPSockets Dreemchest)

add_executable(08_NetworkHandlers 08_NetworkHandlers.cpp)
target_link_libraries(08_NetworkHandlers Dreemchest)

add_executable(09_ModelViewViewModel 09_ModelViewViewModel.cpp)
target_link_libraries(09_ModelViewViewModel Dreemchest)

if (DC_QT_ENABLED)
	add_executable(10_MvvmQt 10_MvvmQt.cpp)
	target_link_libraries(10_MvvmQt Dreemchest)
endif (DC_QT_ENABLED)

add_executable(11_Particles 11_Particles.cpp)
target_link_libraries(11_Particles Dreemchest)

if (DC_SOUND_ENABLED)
	add_executable(12_Sound 12_Sound.cpp)
	target_link_libraries(12_Sound Dreemchest)
endif (DC_SOUND_ENABLED)

add_executable(13_Scene 13_Scene.cpp)
target_link_libraries(13_Scene Dreemchest)