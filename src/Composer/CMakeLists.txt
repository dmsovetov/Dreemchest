# Set the target name
set(TARGET_NAME Composer)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Automatically run uic when needed.
set(CMAKE_AUTOUIC ON)

add_directories(. SRCS)

# Inject the environment options to VisualStudio user configuration
if (MSVC)
	set(USER_FILE ${TARGET_NAME}.vcxproj.user)
	set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${USER_FILE})
	configure_file(VisualStudio.user ${USER_FILE} @ONLY)
endif (MSVC)

# Setup the Qt library
if (DC_QT_SUPPORT MATCHES "Qt4")
	find_package(Qt4 REQUIRED QtGui QtOpenGL)
	set(QT_LIBRARIES Qt4::QtGui Qt4::QtOpenGL)
	
elseif (DC_QT_SUPPORT MATCHES "Qt5")
	find_package(Qt5Widgets)
	find_package(Qt5OpenGL)
	set(QT_LIBRARIES Qt5::Widgets Qt5::OpenGL)
	
endif (DC_QT_SUPPORT MATCHES "Qt4")

# Create the Composer executable
add_executable(${TARGET_NAME} WIN32 ${SRCS})
 
# Link dependencies
target_link_libraries(${TARGET_NAME} ${QT_LIBRARIES} Dreemchest)