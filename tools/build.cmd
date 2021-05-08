@echo off
REM Currently we require Visual Studio 2019 for C++20 build targeting Release/x64.
REM
REM TODO: allow specifying compiler version as argument.
REM
REM Supported versions for nostd build:
REM - vs2015 (C++11)
REM - vs2017 (C++14)
REM - vs2019 (C++20)
REM
REM Supported versions for STL build:
REM - vs2017 (C++14)
REM - vs2019 (C++20)
REM

if "%VS_TOOLS_VERSION%" == "" set "VS_TOOLS_VERSION=vs2019"
if "%CMAKE_GEN%"        == "" set "CMAKE_GEN=Visual Studio 16 2019"

pushd %~dp0
setlocal enableextensions
setlocal enabledelayedexpansion
set "ROOT=%~dp0\.."

if ("%CMAKE_ARCH%"=="") (
  set CMAKE_ARCH=x64
)

REM Use preinstalled vcpkg if installed or use our local
if "%VCPKG_ROOT%" neq "" (
  set "VCPKG_CMAKE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
) else (
  set "VCPKG_CMAKE=%CD%\vcpkg\scripts\buildsystems\vcpkg.cmake"
)

REM ********************************************************************
REM Setup compiler environment
REM ********************************************************************
call "%~dp0\vcvars.cmd"

REM ********************************************************************
REM Use cmake
REM ********************************************************************
set "PATH=%PATH%;C:\Program Files\CMake\bin\"

REM ********************************************************************
REM Build with nostd implementation
REM ********************************************************************
set CONFIG=-DWITH_STL:BOOL=OFF %*
set "OUTDIR=%ROOT%\out\%VS_TOOLS_VERSION%\nostd"
call :build_config

REM ********************************************************************
REM Build with STL implementation only for vs2017+
REM ********************************************************************
if "%VS_TOOLS_VERSION%" neq "vs2015" (
  set CONFIG=-DWITH_STL:BOOL=ON %*
  set "OUTDIR=%ROOT%\out\%VS_TOOLS_VERSION%\stl"
  call :build_config
)

popd
REM ********************************************************************


REM ********************************************************************
REM Function that allows to build given build configuration
REM ********************************************************************
:build_config
REM TODO: consider rmdir for clean builds
if not exist "%OUTDIR%" mkdir "%OUTDIR%"
cd "%OUTDIR%"
if ("%VS_TOOLS_VERSION%"=="vs2019") (
  REM Only latest vs2019 generator supports and requires -A parameter
  cmake %ROOT% -G "%CMAKE_GEN%" -A %CMAKE_ARCH% -DCMAKE_TOOLCHAIN_FILE="%VCPKG_CMAKE%" %CONFIG%
) else (
  cmake %ROOT% -G "%CMAKE_GEN%" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_CMAKE%" %CONFIG%
)
set "SOLUTION=%OUTDIR%\opentelemetry-cpp.sln"
msbuild "%SOLUTION%" /p:Configuration=Release /p:VcpkgEnabled=true
exit /b 0
