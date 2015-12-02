
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Automatically run uic when needed.
set(CMAKE_AUTOUIC ON)

add_directories(. SRCS)

# Setup the Qt library
if (DC_QT_SUPPORT MATCHES "Qt5")
	find_package(Qt5Widgets)
	set(QT_LIBRARIES Qt5::Widgets)
elseif (DC_QT_SUPPORT MATCHES "Qt4")
	message(FATAL_ERROR "Sorry, Qt4 is not supported now.")
endif (DC_QT_SUPPORT MATCHES "Qt5")

# Create the Composer executable
add_executable(Composer WIN32 ${SRCS})
 
# Link dependencies
target_link_libraries(Composer ${QT_LIBRARIES} Dreemchest)