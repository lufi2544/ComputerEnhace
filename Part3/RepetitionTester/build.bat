@echo off

set SRC_DIR=%~dp0/source
set BIN_DIR=%~dp0/bin
set CFLAGS=/std:c++17 /Zi
set SRC_FILES=part3.cpp
set CE_DIR=../../../../ComputerEnhace
set CE_DIR_1=%CE_DIR%/Part1
set CE_DIR_2=%CE_DIR%/Part2


if not exist %BIN_DIR% mkdir %BIN_DIR%
cd %BIN_DIR%

call "setup_cl_x64.bat"

cl /Fe:RepetitionTester.exe %CFLAGS% -I%SRC_DIR% -I%CE_DIR_1% -I%CE_DIR_2% -I%CE_DIR% %SRC_DIR%\%SRC_FILES% /nologo
