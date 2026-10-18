@echo off
rem Usage: run_tests.bat [debug|release] [quick|full], release full by default
rem Builds the configuration first.
setlocal
rem Captured before parsing: shift also shifts %0
set "SCRIPT_DIR=%~dp0"
set "CONFIG=release"
set "SUITE=full"

:parse_args
if "%~1"=="" goto run
if /i "%~1"=="quick" (set "SUITE=quick") else if /i "%~1"=="full" (set "SUITE=full") else (set "CONFIG=%~1")
shift
goto parse_args

:run
call "%SCRIPT_DIR%build.bat" %CONFIG% || exit /b 1
cd /d "%SCRIPT_DIR%.."
ctest --preset %CONFIG%-%SUITE%
exit /b %errorlevel%
