@echo off

set PLATFORM=Windows

call env.bat
call select-project-folder.bat

cmake ..\..\Source -G "Visual Studio 12" 							^
	-DDC_BUILD_TESTS=OFF 											^
	-DDC_WITH_RELIGHT=OFF											^
	-DDC_QT_SUPPORT=Auto 											^
	-DDC_COMPOSER_ENABLED=ON 										^
	-DDC_USE_PCH=ON													^
	-DTIFF_LIBRARY=../../Externals/lib/Windows/120/Debug/tiffd.lib 	^
	-DTIFF_INCLUDE_DIR=../../Externals/include/tiff
pause