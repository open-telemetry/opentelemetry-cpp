#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

# Use CMAKE_VERSION env var if set, else default to 3.31.6
CMAKE_VERSION=${CMAKE_VERSION:-3.31.6}
CMAKE_DIR="cmake-$CMAKE_VERSION-linux-x86_64"
CMAKE_TAR="$CMAKE_DIR.tar.gz"
CMAKE_URL="https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/$CMAKE_TAR"

echo "Installing CMake version: $CMAKE_VERSION"

apt-get update && apt-get remove --purge -y cmake || true

apt-get install -y wget tar

wget "$CMAKE_URL"

tar -xzf "$CMAKE_TAR"

mkdir -p /opt/cmake
mv "$CMAKE_DIR" /opt/cmake/cmake

for file in /opt/cmake/cmake/bin/*; do
    ln -sf "$file" /usr/local/bin/$(basename "$file")
done

rm -f "$CMAKE_TAR"

cmake --version
