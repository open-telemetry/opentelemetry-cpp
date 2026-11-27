# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

$SRC_DIR = (Get-Item -Path ".\").FullName
$VCPKG_DIR = Join-Path "$SRC_DIR" "tools/vcpkg"
$VCPKG_TARGET_TRIPLET = if ($env:VCPKG_TARGET_TRIPLET) { $env:VCPKG_TARGET_TRIPLET } else { "x64-windows" }
# The default matches the vcpkg toolchain's classic-mode installed dir
$VCPKG_INSTALLED_DIR = if ($env:VCPKG_INSTALLED_DIR) { $env:VCPKG_INSTALLED_DIR } else { Join-Path "$VCPKG_DIR" "installed" }

git submodule update --init --depth 1 "tools/vcpkg"
& "$VCPKG_DIR/bootstrap-vcpkg.bat" -disableMetrics

# Use vcpkg's pinned CMake 3.x; CMake 4.x rejects options in some pinned ports
$env:VCPKG_FORCE_DOWNLOADED_BINARIES = "1"

& "$VCPKG_DIR/vcpkg.exe" install `
  "--x-manifest-root=$SRC_DIR/install/vcpkg" `
  "--x-install-root=$VCPKG_INSTALLED_DIR" `
  "--overlay-triplets=$SRC_DIR/install/vcpkg/triplets" `
  "--triplet=$VCPKG_TARGET_TRIPLET" `
  "--host-triplet=$VCPKG_TARGET_TRIPLET"
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}
