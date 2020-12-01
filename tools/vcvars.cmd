@echo off
REM
REM Make sure to enable the 'Visual C++ ATL' components for all platforms during the setup.
REM
REM This build script auto-detects and configures Visual Studio in the following order:
REM 1. Visual Studio 2017 Enterprise
REM 2. Visual Studio 2017 BuildTools
REM 3. Visual Studio 2019 Enterprise
REM 4. Visual Studio 2019 Community
REM 5. Visual Studio 2019 BuildTools
REM

REM 1st parameter - Visual Studio version
if "%1" neq "" (
  goto %1
)

if "%VS_TOOLS_VERSION%" neq "" (
  goto %VS_TOOLS_VERSION%
)

REM vs2017 Enterprise
:vs2017
:vs2017_enterprise
set TOOLS_VS2017_ENTERPRISE="%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\VsDevCmd.bat"
if exist %TOOLS_VS2017_ENTERPRISE% (
  echo Building with vs2017 Enterprise...
  call %TOOLS_VS2017_ENTERPRISE%
  goto tools_configured
)

REM vs2017 BuildTools
:vs2017_buildtools
set TOOLS_VS2017="%ProgramFiles(x86)%\Microsoft Visual Studio\2017\BuildTools\Common7\Tools\VsDevCmd.bat"
if exist %TOOLS_VS2017% (
  echo Building with vs2017 BuildTools...
  call %TOOLS_VS2017%
  goto tools_configured
)

REM vs2019 Enterprise
:vs2019
:vs2019_enterprise
set TOOLS_VS2019_ENTERPRISE="%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise\Common7\Tools\VsDevCmd.bat"
if exist %TOOLS_VS2019_ENTERPRISE% (
  echo Building with vs2019 Enterprise...
  call %TOOLS_VS2019_ENTERPRISE%
  goto tools_configured
)

REM vs2019 Community
:vs2019_community
set TOOLS_VS2019_COMMUNITY="%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
if exist %TOOLS_VS2019_COMMUNITY% (
  echo Building with vs2019 Community...
  call %TOOLS_VS2019_COMMUNITY%
  goto tools_configured
)

REM vs2019 BuildTools
:vs2019_buildtools
set TOOLS_VS2019="%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\Common7\Tools\VsDevCmd.bat"
if exist %TOOLS_VS2019% (
  echo Building with vs2019 BuildTools...
  call %TOOLS_VS2019%
  goto tools_configured
)

REM vs2015
:vs2015
set TOOLS_VS2015="%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"
if exist %TOOLS_VS2015% (
  echo Building with vs2015 BuildTools...
  call %TOOLS_VS2015%
  set "VCPKG_VISUAL_STUDIO_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio 14.0"
  set VCPKG_PLATFORM_TOOLSET=v140
  goto tools_configured
)

echo WARNING:*********************************************
echo WARNING: cannot auto-detect Visual Studio version !!!
echo WARNING:*********************************************
set TOOLS_VS_NOTFOUND=1
exit /b 0

:tools_configured
