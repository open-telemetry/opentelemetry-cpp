@echo off
set "PATH=%PATH%;%~dp0;%~dp0\vcpkg"
pushd %~dp0

REM Fail if chocolatey is not installed
where choco >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
  echo This script requires chocolatey. Installation instructions: https://chocolatey.org/docs/installation
  exit -1
)
set ERRORLEVEL=0

REM Print current Visual Studio installations detected
where vswhere >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
  echo Visual Studio installations detected:
  vswhere -property installationPath
)

REM Install tools needed for building stuff
choco install -y cmake
choco install -y svn
choco install -y git
choco install -y llvm
choco install -y zip

REM Try to autodetect Visual Studio
call "%~dp0\vcvars.cmd"
if "%TOOLS_VS_NOTFOUND%" == "1" (
  REM Cannot detect MSBuild path
  REM TODO: no command line tools..
  REM TODO: use MSBuild from vswhere?
)

REM If vcpkg is not ready, then build it from source
where vcpkg >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
  call bootstrap-vcpkg.bat
)

vcpkg integrate install
vcpkg install gtest:x64-windows
vcpkg install --overlay-ports=%~dp0\ports benchmark:x64-windows
vcpkg install ms-gsl:x64-windows

popd
exit /b 0
