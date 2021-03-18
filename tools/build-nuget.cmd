@echo off
pushd "%~dp0"
set "PATH=%CD%;%PATH%"
if not exist "..\packages" mkdir "..\packages"
if not defined PackageVersion (
 for /F "tokens=*" %%i in ('git describe --tags') do (
   set PackageVersion=%%i
 )
)
powershell .\build-nuget.ps1
popd
