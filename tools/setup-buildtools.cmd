set "PATH=C:\Windows;C:\Windows\System32;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Program Files\Git\bin;%~dp0\vcpkg;%PATH%"
cd %~dp0
where choco >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
  echo This script requires chocolatey. Installation instructions: https://chocolatey.org/docs/installation
  exit -1
)

choco install -y cmake
choco install -y svn
choco install -y git
choco install -y llvm
choco install -y zip
choco install -y visualstudio2019enterprise

call "%~dp0\vcvars.cmd"
call bootstrap-vcpkg.bat
vcpkg integrate install
vcpkg install gtest:x64-windows
vcpkg install benchmark:x64-windows
vcpkg install ms-gsl:x64-windows

exit /b 0
