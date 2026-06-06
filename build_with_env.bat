@echo off
REM Loads the MSVC x64 toolchain environment, then runs the project build.
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
call build.bat
