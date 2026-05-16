@echo off
cls

REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
REM Root & Third Party Paths
REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set "ROOT=%~dp0"
set "TP=%ROOT%third_party"

set "GLEW_INC=%TP%\glew\include"
set "GLEW_LIB=%TP%\glew\lib\Release\x64"

set "OPENAL_INC=%TP%\openal\include"
set "OPENAL_LIB=%TP%\openal\libs\Win64"

set "ASSIMP_INC=%TP%\assimp\include"
set "ASSIMP_LIB=%TP%\assimp\lib"

set "GLM_INC=%TP%\glm"

set "FFMPEG_INC=%TP%\ffmpeg\include"
set "FFMPEG_LIB=%TP%\ffmpeg\lib"

set "RUNTIME_DIR=%TP%\runtime"

REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
REM CUDA (system installed)
REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set "CUDA_ROOT=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.1"
if not exist "%CUDA_ROOT%\bin\nvcc.exe" (
    set "CUDA_ROOT=%CUDA_PATH%"
)

if "%CUDA_ROOT%"=="" (
    echo [ERROR] CUDA toolkit not found.
    pause
    exit /b 1
)

REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
REM Create bin folder
REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if not exist "bin" mkdir "bin"

echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo Cleaning previous build...
del /q OGLMain.exe 2>nul
del /q ".\bin\*.obj" 2>nul
del /q ".\bin\OGLMain.res" 2>nul

echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo Compiling C++ files...

for /r ".\src" %%i in (*.cpp) do (
    echo [C++] %%i

    cl.exe /Fo".\bin\\" /c /EHsc /MD ^
        /I "%GLEW_INC%" ^
        /I "%OPENAL_INC%" ^
        /I "%ASSIMP_INC%" ^
        /I "%GLM_INC%" ^
        /I "%CUDA_ROOT%\include" ^
        /I ".\src\imgui" ^
        /I "%FFMPEG_INC%" ^
        "%%i"

    if errorlevel 1 (
        echo [ERROR] Compilation failed: %%i
        pause
        exit /b 1
    )
)

echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo Compiling resources...

rc.exe /Fo".\bin\OGLMain.res" .\src\OGLMain.rc

if errorlevel 1 (
    echo [ERROR] Resource compile failed
    pause
    exit /b 1
)

echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo Compiling CUDA...

"%CUDA_ROOT%\bin\nvcc.exe" -c -Xcompiler "/MD" ^
    -I"%CUDA_ROOT%\include" ^
    -o ".\bin\TerrainGPU.obj" ^
    ".\src\effects\terrain\TerrainGPU.cu"

if errorlevel 1 (
    echo [ERROR] CUDA compile failed
    pause
    exit /b 1
)

echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo Linking...

link.exe /out:OGLMain.exe ".\bin\*.obj" ".\bin\OGLMain.res" ^
    /LIBPATH:"%GLEW_LIB%" ^
    /LIBPATH:"%OPENAL_LIB%" ^
    /LIBPATH:"%ASSIMP_LIB%" ^
    /LIBPATH:"%CUDA_ROOT%\lib\x64" ^
    /LIBPATH:"%FFMPEG_LIB%" ^
    glew32.lib ^
    OpenAL32.lib ^
    assimp-vc143-mtd.lib ^
    user32.lib gdi32.lib opengl32.lib glu32.lib ^
    avformat.lib avcodec.lib swscale.lib avutil.lib ^
    /SUBSYSTEM:WINDOWS

if errorlevel 1 (
    echo [ERROR] Linking failed
    pause
    exit /b 1
)

echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo BUILD SUCCESSFUL
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
REM Setup runtime PATH (NO DLL COPY)
REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set PATH=%RUNTIME_DIR%;%PATH%

@REM echo Running application...
@REM echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@REM OGLMain.exe
