@echo off

set PROJECTS=..\Projects
set QT4=D:\SDK\Qt\4.8.6
set QT5=D:\SDK\Qt\5.5
set CMAKEHOME=C:\Program Files (x86)\CMake\bin

set CMAKE_PREFIX_PATH=%QT4%;%QT5%
set PATH=%CMAKEHOME%