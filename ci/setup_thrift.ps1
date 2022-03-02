$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

# TODO: consider moving this to .gitmodules
git clone --depth=1 -b 2020.04 https://github.com/Microsoft/vcpkg.git
Push-Location -Path vcpkg
$VCPKG_DIR=(Get-Item -Path ".\").FullName
setx VCPKG_DIR (Get-Item -Path ".\").FullName
./bootstrap-vcpkg.bat

# boost needed for thrift
./vcpkg install boost-predef[core]:x64-windows boost-locale[core]:x64-windows boost-numeric-conversion[core]:x64-windows boost-scope-exit[core]:x64-windows  openssl:x64-windows

Pop-Location