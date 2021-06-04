@echo off
REM ##########################################################################################
REM # Build SDK with (msvc or clang) + CMake + (MSBuild or Ninja).                           #
REM #                                                                                        #
REM # CMake arguments may be passed as parameters to this script.                            #
REM # If Visual Studio is not installed, then this script falls back to LLVM-CLang,          #
REM # Emscripten or any other C++ compiler of your choice.                                   #
REM #                                                                                        #
REM ##########################################################################################
REM #                                                                                        #
REM # Options passed as environment variables:                                               #
REM #                                                                                        #
REM # BUILDTOOLS_VERSION - specify build tools version. See `vcvars.cmd` for details.        #
REM # CMAKE_GEN          - specify CMake generator.                                          #
REM # VCPKG_ROOT         - path to vcpkg root                                                #
REM # ARCH               - architecture to build for (default: x64)                          #
REM #                                                                                        #
REM ##########################################################################################
set "PATH=%PATH%;%ProgramFiles%\CMake\bin"
pushd %~dp0
setlocal enableextensions
setlocal enabledelayedexpansion
if not defined BUILDTOOLS_VERSION (
  set BUILDTOOLS_VERSION=vs2019
)

REM ##########################################################################################
REM Set up CMake generator. Use Ninja if available.
REM ##########################################################################################
for /f "tokens=*" %%F in ('where ninja') do (
  set NINJA=%%F
)

if defined VCPKG_ROOT (
  if not defined NINJA (
    for /f "tokens=*" %%F in ('where /R %VCPKG_ROOT%\vcpkg\downloads\tools ninja') do (
      set NINJA=%%F
    )
    popd
  )
)

if not defined NINJA (
  for /f "tokens=*" %%F in ('where /R %CD%\vcpkg\downloads\tools ninja') do (
    set NINJA=%%F
  )
)

if defined NINJA (
  echo Found ninja: !NINJA!
  if not defined CMAKE_GEN (
    set CMAKE_GEN=Ninja
  )
)

if not defined CMAKE_GEN (
  set CMAKE_GEN=Visual Studio 16 2019
)

set "ROOT=%~dp0\.."
if not defined ARCH (
  set ARCH=x64
)

REM ##########################################################################################
REM Use preinstalled vcpkg from %VCPKG_ROOT% if installed or use our local snapshot of it.
REM ##########################################################################################
if defined VCPKG_ROOT (
  set "VCPKG_CMAKE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
) else (
  set "VCPKG_CMAKE=%CD%\vcpkg\scripts\buildsystems\vcpkg.cmake"
)

REM ##########################################################################################
REM Setup Microsoft Visual C++ compiler environment (if found, if not - fallback to alternate)
REM ##########################################################################################
call "%~dp0\vcvars.cmd"

REM Prefer Visual Studio C++ compiler if found
for /f "tokens=*" %%F in ('where cl.exe') do (
  set CONFIG=!CONFIG! -DCMAKE_C_COMPILER:FILEPATH="%%F" -DCMAKE_CXX_COMPILER:FILEPATH="%%F"
  echo !CONFIG!
)

REM ##########################################################################################
REM The following two configurations are built below:
REM - nostd            - build with OpenTelemetry C++ Template library
REM - stl              - build with Standard Template Library
REM ##########################################################################################
REM Build with nostd implementation. Supported BUILDTOOLS_VERSION:
REM - vs2015 (C++11)
REM - vs2017 (C++14)
REM - vs2019 (C++20)
REM ##########################################################################################
set CONFIG=-DWITH_STL:BOOL=OFF %*
set "OUTDIR=%ROOT%\out\%BUILDTOOLS_VERSION%\nostd"
call :build_config

REM ##########################################################################################
REM Build with STL implementation (only for vs2017+). Supported BUILDTOOLS_VERSION:
REM - vs2017 (C++14)
REM - vs2019 (C++20) - optimal config with all OpenTelemetry API classes using STL only.
REM ##########################################################################################
if "%BUILDTOOLS_VERSION%" neq "vs2015" (
  set CONFIG=-DWITH_STL:BOOL=ON %*
  set "OUTDIR=%ROOT%\out\%BUILDTOOLS_VERSION%\stl"
  call :build_config
)

popd
exit
REM ##########################################################################################
REM Function that allows to build given build configuration with MSBuild or Ninja
REM ##########################################################################################
:build_config
REM TODO: consider rmdir for clean builds
if not exist "%OUTDIR%" mkdir "%OUTDIR%"
cd "%OUTDIR%"

REM Prefer ninja if available
if "!CMAKE_GEN!" == "Ninja" (
  call :build_config_ninja
  exit /b
)

if "!BUILDTOOLS_VERSION!" == "vs2019" (
  REM Only latest vs2019 generator supports and requires -A parameter
  cmake -G "!CMAKE_GEN!" -A !ARCH! -DCMAKE_TOOLCHAIN_FILE="!VCPKG_CMAKE!" !CONFIG! "!ROOT!"
) else (
  REM Old vs2017 generator does not support -A parameter
  cmake -G "!CMAKE_GEN!" -DCMAKE_TOOLCHAIN_FILE="!VCPKG_CMAKE!" !CONFIG! "!ROOT!"
)
set "SOLUTION=%OUTDIR%\opentelemetry-cpp.sln"
msbuild "%SOLUTION%" /p:Configuration=Release /p:VcpkgEnabled=true
exit /b

REM ##########################################################################################
REM Build using CMake+ninja: vs2019 is known to work well. vs2017 was not tested.
REM ##########################################################################################
REM
REM Optional parameters may be passed to `build.cmd ARG1 ARG2 .. ARGN`.
REM
REM These arguments get appended to CONFIG and passed to CMake.
REM 
REM To build for Debug:
REM -DCMAKE_BUILD_TYPE:STRING="Debug"
REM 
REM To specify alternate installation path:
REM -DCMAKE_INSTALL_PREFIX:PATH=C:\path\to\install
REM 
REM To specify alternate toolchain version:
REM -DCMAKE_C_COMPILER:FILEPATH="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Tools/MSVC/14.28.29910/bin/Hostx64/x64/cl.exe"
REM -DCMAKE_CXX_COMPILER:FILEPATH="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Tools/MSVC/14.28.29910/bin/Hostx64/x64/cl.exe"
REM 
REM To specify alternate version of Ninja.exe:
REM -DCMAKE_MAKE_PROGRAM="C:\PROGRAM FILES (X86)\MICROSOFT VISUAL STUDIO\2019\ENTERPRISE\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\Ninja\ninja.exe"
REM 
REM ##########################################################################################
:build_config_ninja
cmake -G "Ninja" -DCMAKE_MAKE_PROGRAM="!NINJA!" -DCMAKE_TOOLCHAIN_FILE="!VCPKG_CMAKE!" !CONFIG! "!ROOT!"
%NINJA%
exit /b
