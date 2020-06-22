$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

git clone https://github.com/Microsoft/vcpkg.git
Push-Location -Path vcpkg
$VCPKG_DIR=(Get-Item -Path ".\").FullName
./bootstrap-vcpkg.bat
./vcpkg integrate install
./vcpkg install benchmark:x64-windows
./vcpkg install gtest:x64-windows
Pop-Location
