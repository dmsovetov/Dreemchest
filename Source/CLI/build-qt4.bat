@echo off

set CURRENT_DIR=%~dp0

call env.bat
cd %QT4%
configure -confirm-license -opensource -no-phonon -no-phonon-backend -no-webkit -no-openvg -nomake demos -nomake examples
set CL=/MP
nmake
cd %CURRENT_DIR%
pause