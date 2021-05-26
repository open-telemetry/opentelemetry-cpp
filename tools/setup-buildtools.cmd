@echo off
setlocal enableextensions
setlocal enabledelayedexpansion
set "PATH=%ProgramFiles%\CMake\bin;%~dp0;%~dp0vcpkg;%ProgramData%\chocolatey\bin;%PATH%"
if "%VCPKG_ROOT%" NEQ "" set "PATH=%VCPKG_ROOT%;%PATH%"
pushd %~dp0

net session >nul 2>&1
if %errorLevel% == 0 (
  echo Running with Administrative privilege...
  REM Fail if chocolatey is not installed
  where /Q choco
  if %ERRORLEVEL% == 1 (
    echo This script requires chocolatey. Installation instructions: https://chocolatey.org/docs/installation
    exit -1
  )
  REM Install tools needed for building, but only if not installed yet
  where /Q vswhere || choco install -y vswhere
  where /Q cmake || choco install -y cmake
  where /Q git || choco install -y git
) else (
  echo Running without Administrative privilege...
)

REM Print current Visual Studio installations detected
where /Q vswhere
if %ERRORLEVEL% == 0 (
  echo Visual Studio installations detected:
  vswhere -property installationPath
)

REM This script allows to pass architecture in ARCH env var
if not defined ARCH (
  set ARCH=x64
)

REM Try to autodetect Visual Studio
call "%~dp0vcvars.cmd"
if "%TOOLS_VS_NOTFOUND%" == "1" (
  echo WARNING: cannot autodetect Visual Studio installation!
)

where /Q vcpkg.exe
if %ERRORLEVEL% == 1 (
  REM Build our own vcpkg from source
  REM Prefer building in VCPKG_ROOT
  if not defined VCPKG_ROOT (
    set "VCPKG_ROOT=%~dp0\vcpkg"
  )
  pushd "!VCPKG_ROOT!"
  call bootstrap-vcpkg.bat
  popd
) else (
  echo Using existing vcpkg installation...
)

REM Install dependencies
vcpkg install gtest:%ARCH%-windows
vcpkg install --head --overlay-ports=%~dp0ports benchmark:%ARCH%-windows
vcpkg install ms-gsl:%ARCH%-windows
vcpkg install nlohmann-json:%ARCH%-windows
vcpkg install abseil:%ARCH%-windows
vcpkg install protobuf:%ARCH%-windows
vcpkg install gRPC:%ARCH%-windows
vcpkg install prometheus-cpp:%ARCH%-windows
vcpkg install curl:%ARCH%-windows
popd
exit /b 0
