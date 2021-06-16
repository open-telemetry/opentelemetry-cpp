#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e
export DEBIAN_FRONTEND=noninteractive

BUILD_DIR=/tmp/
INSTALL_DIR=/usr/local/
TAG=20210324.0
pushd $BUILD_DIR
git clone --depth=1 -b ${TAG} https://github.com/abseil/abseil-cpp.git
cd abseil-cpp
mkdir build && pushd build
cmake -DBUILD_TESTING=OFF -DCMAKE_CXX_STANDARD=11 \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
    ..
make -j $(nproc)
make install
popd
popd
export PATH=${INSTALL_DIR}/bin:$PATH  # ensure to use the installed abseil
