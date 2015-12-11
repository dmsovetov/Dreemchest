@echo off

set CURRENT_DIR=%~dp0

call env.bat
cd %QT5%
configure -confirm-license -opensource -opengl desktop -no-openvg -nomake examples -nomake tests -prefix %QT5%
set CL=/MP
nmake
cd %CURRENT_DIR%
pause