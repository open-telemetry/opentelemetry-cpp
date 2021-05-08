@echo off
set "PATH=%ProgramFiles%\CMake\bin;%~dp0;%~dp0vcpkg;%PATH%"
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

REM Try to autodetect Visual Studio
call "%~dp0vcvars.cmd" x64
if "%TOOLS_VS_NOTFOUND%" == "1" (
  REM Cannot detect MSBuild path
  REM TODO: no command line tools..
  REM TODO: use MSBuild from vswhere?
)

where /Q vcpkg.exe
if %ERRORLEVEL% == 1 (
  REM Build our own vcpkg from source
  pushd .\vcpkg
  call bootstrap-vcpkg.bat
  popd
) else (
  echo Using existing vcpkg installation...
)

REM Install dependencies
vcpkg install gtest:x64-windows
vcpkg install --head --overlay-ports=%~dp0ports benchmark:x64-windows
vcpkg install ms-gsl:x64-windows
vcpkg install nlohmann-json:x64-windows
vcpkg install abseil:x64-windows
vcpkg install protobuf:x64-windows
vcpkg install gRPC:x64-windows
vcpkg install prometheus-cpp:x64-windows
popd
exit /b 0
