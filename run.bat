@echo off
REM Runs the already-built OGLMain.exe with the third-party runtime DLLs on PATH.
set "ROOT=%~dp0"
set "PATH=%ROOT%third_party\runtime;%PATH%"
OGLMain.exe
