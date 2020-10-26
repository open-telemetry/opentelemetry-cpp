set "VS_TOOLS_VERSION=vs2015"
REM Ref. https://cmake.org/cmake/help/latest/generator/Visual%20Studio%2014%202015.html
set "CMAKE_GEN=Visual Studio 14 2015 Win64"
cd %~dp0
call setup-buildtools.cmd
call build.cmd -DWITH_ABSEIL:BOOL=ON
