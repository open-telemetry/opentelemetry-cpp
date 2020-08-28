@echo off
set "PATH=%PATH%;%~dp0;%~dp0\vcpkg"
pushd %~dp0

REM Fail if chocolatey is not installed
where /Q choco
if ERRORLEVEL 1 (
  echo This script requires chocolatey. Installation instructions: https://chocolatey.org/docs/installation
  exit -1
)

REM Print current Visual Studio installations detected
where /Q vswhere
if ERRORLEVEL 0 (
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

where /Q vcpkg.exe
if ERRORLEVEL 1 (
  REM Build our own vcpkg from source
  pushd .\vcpkg
  call bootstrap-vcpkg.bat
  popd
)

REM Install it
vcpkg integrate install
vcpkg install gtest:x64-windows
vcpkg install --overlay-ports=%~dp0\ports benchmark:x64-windows
vcpkg install ms-gsl:x64-windows
vcpkg install nlohmann-json:x64-windows
popd
exit /b 0
