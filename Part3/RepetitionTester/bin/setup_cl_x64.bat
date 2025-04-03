@echo off

REM (allen): quit early if we already have cl
where /q cl
IF %ERRORLEVEL% == 0 (EXIT /b)

SET SCRIPTS_PATH=%~dp0
call "setup_cl_generic.bat" amd64

