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

cd %~dp0
setlocal enableextensions
setlocal enabledelayedexpansion
set "ROOT=%~dp0\.."
set "VCPKG_CMAKE=%CD%/vcpkg/scripts/buildsystems/vcpkg.cmake"

call "%~dp0\vcvars.cmd"

REM ********************************************************************
REM Use cmake
REM ********************************************************************
set "PATH=C:\Program Files\CMake\bin\;%PATH%"

REM Build with nostd implementation
set CONFIG=-DWITH_STL:BOOL=OFF
set "OUTDIR=%ROOT%\out.nostd"
call :build_config

REM Build with STL implementation
set CONFIG=-DWITH_STL:BOOL=ON
set "OUTDIR=%ROOT%\out.stl"
call :build_config

exit /B %ERRORLEVEL% 

:build_config
mkdir "%OUTDIR%"
cd "%OUTDIR%"
REM Optional platform specification parameter below: -Ax64
cmake ../ -G "%CMAKE_GEN%" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_CMAKE%" %CONFIG%
set "SOLUTION=%OUTDIR%\opentelemetry-cpp.sln"
msbuild "%SOLUTION%" /p:Configuration=Release /p:Platform=x64
exit /b 0
