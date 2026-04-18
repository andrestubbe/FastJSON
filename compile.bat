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

REM Check for Visual Studio
where cl >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo %RED%Error: MSVC compiler not found%RESET%
    echo %YELLOW%Please run this script from a Visual Studio Developer Command Prompt%RESET%
    exit /b 1
)

REM Find Java JNI headers
if not defined JAVA_HOME (
    for /f "tokens=*" %%i in ('where java') do set JAVA_BIN=%%i
    for %%i in ("!JAVA_BIN!") do set JAVA_DIR=%%~dpi
    for %%i in ("!JAVA_DIR!..") do set JAVA_HOME=%%~fi
)

if not exist "%JAVA_HOME%\include\jni.h" (
    echo %RED%Error: JAVA_HOME not set or jni.h not found%RESET%
    echo %YELLOW%Please set JAVA_HOME environment variable%RESET%
    exit /b 1
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
set COMMON_FLAGS=/O2 /arch:AVX2 /EHsc /W3 /MD /DNDEBUG /nologo

REM Include paths
set INCLUDES=/I "%JAVA_HOME%\include" /I "%JAVA_HOME%\include\win32" /I "native"

echo.
echo %YELLOW%Compiling with AVX2 SIMD optimizations...%RESET%
echo %YELLOW%Flags: %COMMON_FLAGS%%RESET%
echo.

REM Compile
echo Compiling fastjson.cpp...
cl %COMMON_FLAGS% %INCLUDES^ /c /Fo%BUILD_DIR%\fastjson.obj native\fastjson.cpp

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
