REM Build with Visual Studio 2015
set "PATH=%ProgramFiles(x86)%\MSBuild\14.0\Bin;%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\VC\bin;%PATH%"
set BUILDTOOLS_VERSION=vs2015
set ARCH=x64
if NOT "%1"=="" (
  set ARCH=%1
)
set "CMAKE_GEN=Ninja"
set "VCPKG_VISUAL_STUDIO_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\"
set CMAKE_SYSTEM_VERSION=8.1
set "PATH=%ProgramFiles(x86)%\Windows Kits\8.1\bin\%ARCH%;%PATH%"
REM set WINSDK_VERSION=10.0.19041.0
set WINSDK_VERSION=8.1
REM set "PATH=%ProgramFiles(x86)%\Windows Kits\10\bin\10.0.19041.0\%ARCH%\;%PATH%"
set VCPKG_ROOT=C:\work\vcpkg.vs2015
REM set VCPKG_FORCE_SYSTEM_BINARIES=1
cd %~dp0
call setup-buildtools.cmd
call build.cmd -DMSVC_TOOLSET_VERSION=140
