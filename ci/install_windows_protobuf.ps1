$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

cd vcpkg
# Lock to specific version of Protobuf port file to avoid build break
./vcpkg install --overlay-ports="$PSScriptRoot\ports" protobuf:x64-windows
