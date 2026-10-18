@echo off
rem Usage: build.bat [debug|release], release by default
setlocal
set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=release"

where cl >nul 2>nul
if errorlevel 1 (
	set "VCVARS=%ProgramFiles%\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat"
	call :setup_msvc || exit /b 1
)

cd /d "%~dp0.."
cmake --preset %CONFIG% || exit /b 1
cmake --build --preset %CONFIG% || exit /b 1
exit /b 0

:setup_msvc
if not exist "%VCVARS%" (
	echo cl is not on PATH and %VCVARS% does not exist
	exit /b 1
)
call "%VCVARS%" amd64 >nul
exit /b %errorlevel%
