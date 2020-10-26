set "VS_TOOLS_VERSION=vs2019"
set "CMAKE_GEN=Visual Studio 16 2019"
cd %~dp0
call setup-buildtools.cmd
call build.cmd
