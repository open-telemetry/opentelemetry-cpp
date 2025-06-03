#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

INSTALL_PREFIX="/usr/local"

LLVM_VERSION=$(llvm-config --version 2>/dev/null | cut -d. -f1)

if [ -z "$LLVM_VERSION" ]; then
    echo "Error: LLVM not found. Exiting."
    exit 1
fi

echo "LLVM_VERSION=$LLVM_VERSION"
echo "Installing IWYU..."

cd /tmp
git clone --depth 1 --branch clang_$LLVM_VERSION https://github.com/include-what-you-use/include-what-you-use.git
cd include-what-you-use
mkdir -p build
cd build

cmake -DCMAKE_PREFIX_PATH=/usr/lib/llvm-$LLVM_VERSION -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX ..
make -j$(nproc)
make install

echo "IWYU install complete. Verifying installation..."
include-what-you-use --version
