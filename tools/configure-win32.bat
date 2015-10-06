@echo off

set PROJECTS=..\Projects

if exist %PROJECTS%\Windows (
	rmdir /s /q %PROJECTS%\Windows
)

mkdir %PROJECTS%\Windows
cd %PROJECTS%\Windows
cmake ..\..\src -G "Visual Studio 12" -DDC_BUILD_TESTS=OFF
pause