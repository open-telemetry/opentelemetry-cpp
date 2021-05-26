@echo off
REM +-------------------------------------------------------------------+
REM | Autodetect and set up the build environment for Visual Studio.    |
REM | Visual Studio version may be specified as 1st argument.           |
REM +-------------------------------------------------------------------+
REM | Description                             | Argument value          |
REM +-----------------------------------------+-------------------------+
REM | Autodetect Visual Studio 2019           | vs2019                  |
REM | Visual Studio 2019 Enterprise           | vs2019_enterprise       |
REM | Visual Studio 2019 Professional         | vs2019_professional     |
REM | Visual Studio 2019 Community            | vs2019_community        |
REM | Visual Studio 2019 Build Tools (no IDE) | vs2019_buildtools       |
REM |                                         |                         |
REM | Autodetect Visual Studio 2017           | vs2017                  |
REM | Visual Studio 2017 Enterprise           | vs2017_enterprise       |
REM | Visual Studio 2017 Professional         | vs2017_professional     |
REM | Visual Studio 2017 Community            | vs2017_community        |
REM | Visual Studio 2017 Build Tools (no IDE) | vs2017_buildtools       |
REM |                                         |                         |
REM | Visual Studio 2015 Build Tools (no IDE) | vs2015                  |
REM +-----------------------------------------+-------------------------+
set "VSCMD_START_DIR=%CD%"

if not defined ARCH (
  set ARCH=x64
)

if "%1" neq "" (
  goto %1
)

if defined VS_TOOLS_VERSION (
  goto %VS_TOOLS_VERSION%
)

:vs2019
:vs2019_enterprise
set TOOLS_VS2019_ENTERPRISE="%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
if exist %TOOLS_VS2019_ENTERPRISE% (
  echo Building with vs2019 Enterprise...
  call %TOOLS_VS2019_ENTERPRISE% %ARCH%
  goto tools_configured
)

:vs2019_professional
set TOOLS_VS2019_PRO="%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat"
if exist %TOOLS_VS2019_PRO% (
  echo Building with vs2019 Professional...
  call %TOOLS_VS2019_PRO% %ARCH%
  goto tools_configured
)

:vs2019_community
set TOOLS_VS2019_COMMUNITY="%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %TOOLS_VS2019_COMMUNITY% (
  echo Building with vs2019 Community...
  call %TOOLS_VS2019_COMMUNITY% %ARCH%
  goto tools_configured
)

:vs2019_buildtools
set TOOLS_VS2019="%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
if exist %TOOLS_VS2019% (
  echo Building with vs2019 BuildTools...
  call %TOOLS_VS2019% %ARCH%
  goto tools_configured
)

:vs2017
:vs2017_enterprise
set TOOLS_VS2017_ENTERPRISE="%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
if exist %TOOLS_VS2017_ENTERPRISE% (
  echo Building with vs2017 Enterprise...
  call %TOOLS_VS2017_ENTERPRISE% %ARCH%
  goto tools_configured
)

:vs2017_professional
set TOOLS_VS2017_PRO="%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall.bat"
if exist %TOOLS_VS2017_PRO% (
  echo Building with vs2017 Professional...
  call %TOOLS_VS2017_PRO% %ARCH%
  goto tools_configured
)

:vs2017_community
set TOOLS_VS2017_COMMUNITY="%ProgramFiles(x86)%\Microsoft Visual Studio\2017\VC\Auxiliary\Build\vcvarsall.bat"
if exist %TOOLS_VS2017_COMMUNITY% (
  echo Building with vs2017 Community...
  call %TOOLS_VS2017_COMMUNITY% %ARCH%
  goto tools_configured
)

:vs2017_buildtools
set TOOLS_VS2017="%ProgramFiles(x86)%\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
if exist %TOOLS_VS2017% (
  echo Building with vs2017 BuildTools...
  call %TOOLS_VS2017% %ARCH%
  goto tools_configured
)

:vs2015
set TOOLS_VS2015="%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\VC\bin\vcvarsall.bat"
if exist %TOOLS_VS2015% (
  echo Building with vs2015 BuildTools...
  call %TOOLS_VS2015% %ARCH%
  set "VCPKG_VISUAL_STUDIO_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio 14.0"
  set VCPKG_PLATFORM_TOOLSET=v140
  goto tools_configured
)

echo WARNING: cannot auto-detect Visual Studio version !!!
REM Caller may decide what to do if Visual Studio environment
REM is not set up by checking TOOLS_VS_NOTFOUND
set TOOLS_VS_NOTFOUND=1
exit /b 0

:tools_configured
