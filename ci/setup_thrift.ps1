$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

# TODO: consider moving this to .gitmodules
git clone https://github.com/Microsoft/vcpkg.git
Push-Location -Path vcpkg
$VCPKG_DIR=(Get-Item -Path ".\").FullName
setx VCPKG_DIR (Get-Item -Path ".\").FullName
./bootstrap-vcpkg.bat
./vcpkg integrate install

# boost needed for thrift
./vcpkg install boost-locale:x64-windows boost-numeric_conversion:x64-windows

Pop-Location
