$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

./vcpkg install protobuf:x64-windows
