set "PATH=C:\Windows;C:\Windows\System32;C:\Windows\System32\WindowsPowerShell\v1.0;C:\Program Files\CMake\bin"
set "VS_TOOLS_VERSION=vs2017"
set "CMAKE_GEN=Visual Studio 15 2017 Win64"
call setup-buildtools.cmd
call build.cmd
