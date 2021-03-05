REM Build with Visual Studio 2015
set "VS_TOOLS_VERSION=vs2015"
set ARCH=Win64
if NOT "%1"=="" (
  set ARCH=%1
)
set "CMAKE_GEN=Visual Studio 14 2015 %ARCH%"
cd %~dp0
call setup-buildtools.cmd
REM TODO: currently we cannot build without Abseil variant for Visual Studio 2015
call build.cmd -DWITH_ABSEIL:BOOL=ON
