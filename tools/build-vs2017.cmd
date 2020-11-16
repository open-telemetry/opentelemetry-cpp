REM Build with Visual Studio 2017
set "VS_TOOLS_VERSION=vs2017"
set ARCH=Win64
if NOT "%1"=="" (
  set ARCH=%1
)
set "CMAKE_GEN=Visual Studio 15 2017 %ARCH%"
cd %~dp0
call setup-buildtools.cmd
call build.cmd
