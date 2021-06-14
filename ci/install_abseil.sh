#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e
export DEBIAN_FRONTEND=noninteractive

if ! type cmake > /dev/null; then
    #cmake not installed, exiting
    exit 1
fi
export BUILD_DIR=/tmp/
export INSTALL_DIR=/usr/local/
pushd $BUILD_DIR
git clone --depth=1 -b 20210324.0 https://github.com/abseil/abseil-cpp.git
cd abseil-cpp
mkdir build && cd build
pushd build
cmake -DBUILD_TESTING=OFF -DCMAKE_CXX_STANDARD=11 \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
    ..
make -j $(nproc)
make install
popd
export PATH=${INSTALL_DIR}/bin:$PATH  # ensure to use the installed abseil