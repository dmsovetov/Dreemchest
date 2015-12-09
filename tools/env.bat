@echo off

set PROJECTS=..\Projects
set QT4=D:\SDK\Qt\4.8.6
set QT5=D:\SDK\Qt\5.5
set CMAKEHOME=C:\Program Files (x86)\CMake\bin

set JSONCPP_LIBRARY=..\..\externals\lib\Windows\120\Debug\jsoncpp.lib
set JSONCPP_INCLUDE_DIR=..\..\externals\include

set OPENAL_LIBRARY=..\..\externals\lib\Windows\120\Debug\OpenAL32.lib
set OPENAL_INCLUDE_DIR=..\..\externals\include\OpenAL

set VORBIS_LIBRARY=..\..\externals\lib\Windows\120\Debug\libvorbis_static.lib
set VORBISFILE_LIBRARY=..\..\externals\lib\Windows\120\Debug\libvorbisfile_static.lib
set OGG_LIBRARY=..\..\externals\lib\Windows\120\Debug\libogg_static.lib
set OGG_INCLUDE_DIR=..\..\externals\include\ogg
set VORBIS_INCLUDE_DIR=..\..\externals\include\vorbis

set CMAKE_PREFIX_PATH=%QT4%;%QT5%
set PATH=%CMAKEHOME%