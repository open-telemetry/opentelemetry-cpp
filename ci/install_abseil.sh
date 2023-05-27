#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -ex
export DEBIAN_FRONTEND=noninteractive
[ -z "${ABSEIL_CPP_VERSION}" ] && export ABSEIL_CPP_VERSION="20220623.1"

BUILD_DIR=/tmp/
INSTALL_DIR=/usr/local/
pushd $BUILD_DIR
git clone --depth=1 -b ${ABSEIL_CPP_VERSION} https://github.com/abseil/abseil-cpp.git
cd abseil-cpp
mkdir build && pushd build
cmake -DBUILD_TESTING=OFF \
    -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
    ..
make -j $(nproc)
make install
popd
popd
export PATH=${INSTALL_DIR}/bin:$PATH  # ensure to use the installed abseil
