# Converts all source files in a directory to executable targets
function(create_examples dir)
	file(GLOB src ${dir} "*.cpp")

	foreach(file ${src})
		if (NOT ${file} STREQUAL ${dir})
			get_filename_component(target_name ${file} NAME_WE)

			if (ANDROID)
			    add_library(${target_name} SHARED ${file})
			else ()
			    add_executable(${target_name} WIN32 MACOSX_BUNDLE ${file})
			endif ()
			target_link_libraries(${target_name} Dreemchest Framework)
			set_property(TARGET ${target_name} PROPERTY FOLDER "Examples")

			if (IOS)
			    if (IOS_CODESIGN_IDENTITY STREQUAL "")
			        message(FATAL_ERROR "IOS_CODESIGN_IDENTITY variable was not set for ${target_name}")
			    endif ()
                set_target_properties(${target_name} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${IOS_CODESIGN_IDENTITY}")
			endif ()

			install(TARGETS ${target_name} DESTINATION bin)
		endif()
	endforeach()
endfunction()

add_subdirectory(1_Introduction)
#add_subdirectory(2_Scene)

if (DC_SOUND_ENABLED)
    add_subdirectory(3_Sound)
endif (DC_SOUND_ENABLED)

#add_subdirectory(4_Particles)
add_subdirectory(5_Rendering)
#add_subdirectory(6_Networking)
