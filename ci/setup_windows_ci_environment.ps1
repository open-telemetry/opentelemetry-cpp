$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

# TODO: consider moving this to .gitmodules
git clone https://github.com/Microsoft/vcpkg.git
Push-Location -Path vcpkg
$VCPKG_DIR=(Get-Item -Path ".\").FullName
./bootstrap-vcpkg.bat
./vcpkg integrate install

# Patched Google Benchmark can be shared between vs2017 and vs2019 compilers
./vcpkg install --overlay-ports="$PSScriptRoot\ports" benchmark:x64-windows

# Google Test
./vcpkg install gtest:x64-windows

# nlohmann-json
./vcpkg install nlohmann-json:x64-windows

Pop-Location
