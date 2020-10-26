set "VS_TOOLS_VERSION=vs2017"
set "CMAKE_GEN=Visual Studio 15 2017 Win64"
cd %~dp0
call setup-buildtools.cmd
call build.cmd
