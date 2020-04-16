$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

cd vcpkg
./vcpkg install libevent:x64-windows
