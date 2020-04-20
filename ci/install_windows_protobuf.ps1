$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

cd vcpkg
./vcpkg install protobuf:x64-windows
