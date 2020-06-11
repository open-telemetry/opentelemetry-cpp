@echo off
REM
REM Windows build by default uses Visual C++.
REM TODO: add build-cmake-clang.cmd to build with clang
REM
cd %~dp0
setlocal enableextensions
setlocal enabledelayedexpansion
set ROOT=%~dp0\..
set "VCPKG_CMAKE=%CD%/vcpkg/scripts/buildsystems/vcpkg.cmake"

set VS_TOOLS_VERSION=vs2019
set CMAKE_GEN="Visual Studio 16 2019"

call "%~dp0\vcvars.cmd"

REM ********************************************************************
REM Use cmake
REM ********************************************************************
set "PATH=C:\Program Files\CMake\bin\;%PATH%"

cd %ROOT%
mkdir out
cd out

REM By default we generate the project for the older Visual Studio 2017 even if we have newer version installed
cmake ../ -G %CMAKE_GEN% -DCMAKE_TOOLCHAIN_FILE=%VCPKG_CMAKE% -Ax64

set SOLUTION=%ROOT%\out\opentelemetry-cpp.sln
REM msbuild %SOLUTION% /p:Configuration=Debug /p:Platform=x64
msbuild %SOLUTION% /p:Configuration=Release /p:Platform=x64
