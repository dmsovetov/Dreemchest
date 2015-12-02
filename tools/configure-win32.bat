@echo off

set PLATFORM=Windows

call env.bat
call select-project-folder.bat

cmake ..\..\src -G "Visual Studio 12" -DDC_BUILD_TESTS=OFF -DDC_QT_SUPPORT=Qt4 -DDC_COMPOSER_ENABLED=ON
pause