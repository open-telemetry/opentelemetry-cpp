REM
REM TODO:
REM
REM tools\vcpkg\bootstrap-vcpkg.bat
REM vcpkg integrate install
REM
REM Then use:
REM   -DCMAKE_TOOLCHAIN_FILE=C:/work/opentelemetry-cpp/tools/vcpkg/scripts/buildsystems/vcpkg.cmake

@echo off
cd %~dp0
setlocal enableextensions
setlocal enabledelayedexpansion
set ROOT=%~dp0\..

echo Auto-detecting Visual Studio version...
call "%~dp0\vcvars.cmd"

REM ********************************************************************
REM Use cmake
REM ********************************************************************
set "PATH=C:\Program Files\CMake\bin\;%PATH%"

cd %ROOT%
mkdir out
cd out

REM By default we generate the project for the older Visual Studio 2017 even if we have newer version installed
cmake ../ -G "Visual Studio 15 2017 Win64" -DBUILD_TESTING=0 -DCMAKE_TOOLCHAIN_FILE=C:/work/opentelemetry-cpp/tools/vcpkg/scripts/buildsystems/vcpkg.cmake

set SOLUTION=%ROOT%\out\opentelemetry-cpp.sln
REM msbuild %SOLUTION% /p:Configuration=Debug /p:Platform=x64
msbuild %SOLUTION% /p:Configuration=Release /p:Platform=x64

