@echo off

if not exist %PROJECTS%\%PLATFORM% (
	mkdir %PROJECTS%\%PLATFORM%
)

cd %PROJECTS%\%PLATFORM%