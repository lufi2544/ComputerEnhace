set SRC_DIR=C:\Users\juanes.rayo\Desktop\Programming\ComputerEnhace\Part3\RepetitionTester
set BIN_DIR=%SRC_DIR%\bin
set CFLAGS=/std:c++17 /Zi
set SRC_FILES=part3.cpp
set CE_DIR=../../../../ComputerEnhace

if not exist %BIN_DIR% mkdir %BIN_DIR%
cd %BIN_DIR%

call "../../../setup_cl_x64.bat"

cl /Fe:RepetitionTester.exe %CFLAGS% -I%SRC_DIR% -I%CE_DIR% %SRC_DIR%\%SRC_FILES%
