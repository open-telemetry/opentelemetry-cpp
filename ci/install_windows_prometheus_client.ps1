$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

cd vcpkg
./vcpkg install --overlay-ports="$PSScriptRoot\ports" prometheus-cpp:x86-windows
