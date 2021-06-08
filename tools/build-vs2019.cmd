REM Build with Visual Studio 2017
set "BUILDTOOLS_VERSION=vs2019"
set ARCH=Win64
if NOT "%1"=="" (
  set ARCH=%1
)
if "%ARCH%"=="Win64" (
  REM Parameter needed for CMake Visual Studio 2019 generator
  set CMAKE_ARCH=x64
)

set "CMAKE_GEN=Visual Studio 16 2019"
cd %~dp0
call setup-buildtools.cmd
call build.cmd
