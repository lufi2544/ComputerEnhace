@echo off
REM Set paths relative to the current directory
set SRC_DIR=source
set BUILD_DIR=build

REM Create the build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

REM Compile the source file with C++17 standard and debugging information.
REM The -I flags add the include directories:
REM   1. source (for headers in your source directory)
REM   2. ../../ (two directories up from the current folder)
REM   3. ../profiling/source (for headers from the profiling source)
clang++ -std=c++17 -DPROFILER_PROJECT -O0 -g -I"%CD%\%SRC_DIR%" -I"%CD%\..\.." "%SRC_DIR%\main.cpp" -o "%BUILD_DIR%\Profiling.exe"

if errorlevel 1 (
    echo Compilation failed.
    pause
    exit /b 1
) else (
    echo Compilation succeeded.
    pause
)
