# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

if (-not $env:CMAKE_VERSION) { $env:CMAKE_VERSION = "3.31.6" }
$CMAKE_VERSION = $env:CMAKE_VERSION

choco uninstall cmake cmake.install -y --remove-dependencies --skip-autouninstaller --force --no-progress

Write-Host "Installing CMake version $CMAKE_VERSION ..."
choco install cmake --version=$CMAKE_VERSION --allow-downgrade -y --force --no-progress

cmake --version
