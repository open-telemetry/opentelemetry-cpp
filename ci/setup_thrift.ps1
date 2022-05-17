$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

git submodule update -f "tools/vcpkg"
Push-Location -Path "tools/vcpkg"
$VCPKG_DIR = (Get-Item -Path ".\").FullName
setx VCPKG_DIR "$VCPKG_DIR"
./bootstrap-vcpkg.bat

# boost needed for thrift
./vcpkg "--vcpkg-root=$VCPKG_DIR" install boost-predef[core]:x64-windows boost-locale[core]:x64-windows boost-numeric-conversion[core]:x64-windows boost-scope-exit[core]:x64-windows  openssl:x64-windows

Pop-Location