@echo off
REM FastJSON Native Compilation Script
REM Builds fastjson.dll with MSVC and SIMD optimizations

setlocal EnableDelayedExpansion

REM Configuration
set LIBRARY_NAME=fastjson
set BUILD_DIR=build
set JAVA_VERSION=17

REM Colors for output
set RED=[91m
set GREEN=[92m
set YELLOW=[93m
set RESET=[0m

echo %GREEN%========================================%RESET%
echo %GREEN%  FastJSON Native Build%RESET%
echo %GREEN%========================================%RESET%
echo.

REM Find Visual Studio
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo %RED%Error: vswhere.exe not found. Install Visual Studio 2019+.%RESET%
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    set "VS_PATH=%%i"
)

if not defined VS_PATH (
    echo %RED%Error: Visual Studio with C++ tools not found.%RESET%
    exit /b 1
)

echo %GREEN%Found Visual Studio at: %VS_PATH%%RESET%

REM Setup build environment
call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 (
    echo %RED%Error: Failed to setup VC environment%RESET%
    exit /b 1
)

REM Find Java JNI headers
if not defined JAVA_HOME (
    if exist "C:\Program Files\Java\jdk-21.0.12" (
        set "JAVA_HOME=C:\Program Files\Java\jdk-21.0.12"
    ) else (
        set "JAVA_HOME=C:\Program Files\Java\jdk-25"
    )
)
if not exist "%JAVA_HOME%\include\jni.h" (
    set "JAVA_HOME=C:\Program Files\Java\jdk-21.0.12"
)

echo %GREEN%Found Java at: %JAVA_HOME%%RESET%

REM Create build directory
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

REM Compiler flags
REM /O2 - Optimize for speed
REM /arch:AVX2 - Enable AVX2 instructions
REM /favor:INTEL64 - Optimize for Intel64
REM /EHsc - Exception handling
REM /W3 - Warning level 3
REM /MD - Multi-threaded DLL runtime
REM /DNDEBUG - Release mode
set COMMON_FLAGS=/O2 /arch:AVX2 /EHsc /W3 /MD
set "JNI_INCLUDE=%JAVA_HOME%\include"
set "JNI_WIN=%JAVA_HOME%\include\win32"

REM Include paths
set INCLUDES=/I "%JNI_INCLUDE%" /I "%JNI_WIN%" /I "native" /I "..\FastSIMD\native"

echo.
echo %YELLOW%Compiling with AVX2 SIMD optimizations...%RESET%
echo %YELLOW%Flags: %COMMON_FLAGS%%RESET%
echo.

REM Compile
echo Compiling fastjson.cpp...
cl %COMMON_FLAGS% %INCLUDES% /c /Fo%BUILD_DIR%\fastjson.obj native\fastjson.cpp

if %ERRORLEVEL% neq 0 (
    echo %RED%Compilation failed!%RESET%
    exit /b 1
)

REM Link DLL
echo Linking %LIBRARY_NAME%.dll...
link /DLL /OUT:%BUILD_DIR%\%LIBRARY_NAME%.dll %BUILD_DIR%\fastjson.obj /DEF:native\%LIBRARY_NAME%.def /MACHINE:X64 /nologo

if %ERRORLEVEL% neq 0 (
    echo %RED%Linking failed!%RESET%
    exit /b 1
)

echo.
echo %GREEN%========================================%RESET%
echo %GREEN%  Build Successful!%RESET%
echo %GREEN%========================================%RESET%
echo.
echo Output: %BUILD_DIR%\%LIBRARY_NAME%.dll

REM Copy to resources for JAR packaging
if not exist src\main\resources mkdir src\main\resources
if not exist src\main\resources\native mkdir src\main\resources\native
copy /Y %BUILD_DIR%\%LIBRARY_NAME%.dll src\main\resources\native\ >nul
echo Copied to: src\main\resources\native\%LIBRARY_NAME%.dll

REM Show file info
echo.
for %%i in (%BUILD_DIR%\%LIBRARY_NAME%.dll) do (
    echo Size: %%~zi bytes
    echo Modified: %%~ti
)

echo.
echo %GREEN%Next steps:%RESET%
echo   mvn clean package    - Build JAR with native library
echo   mvn test             - Run tests

endlocal
