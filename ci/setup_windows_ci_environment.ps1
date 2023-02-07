# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

git submodule update -f "tools/vcpkg"
Push-Location -Path "tools/vcpkg"
$VCPKG_DIR = (Get-Item -Path ".\").FullName
./bootstrap-vcpkg.bat
./vcpkg integrate install

# Patched Google Benchmark can be shared between vs2017 and vs2019 compilers
./vcpkg "--vcpkg-root=$VCPKG_DIR" install --overlay-ports="$PSScriptRoot\ports" benchmark:x64-windows

# Google Test
./vcpkg "--vcpkg-root=$VCPKG_DIR" install gtest:x64-windows

# nlohmann-json
./vcpkg "--vcpkg-root=$VCPKG_DIR" install nlohmann-json:x64-windows

Pop-Location
