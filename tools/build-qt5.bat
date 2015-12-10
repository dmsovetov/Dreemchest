@echo off

set CURRENT_DIR=%~dp0

call env.bat
cd %QT5%
configure -confirm-license -opensource -no-openvg -nomake examples
set CL=/MP
nmake
cd %CURRENT_DIR%
pause