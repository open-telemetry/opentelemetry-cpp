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
ABSEIL_CPP_BUILD_OPTIONS=(
    "-DBUILD_TESTING=OFF"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_INSTALL_PREFIX=$INSTALL_DIR"
)

if [ ! -z "${CXX_STANDARD}" ]; then
    ABSEIL_CPP_BUILD_OPTIONS=(${ABSEIL_CPP_BUILD_OPTIONS[@]} "-DCMAKE_CXX_STANDARD=${CXX_STANDARD}")
fi

mkdir build && pushd build
cmake ${ABSEIL_CPP_BUILD_OPTIONS[@]} ..
make -j $(nproc)
make install
popd
popd
export PATH=${INSTALL_DIR}/bin:$PATH  # ensure to use the installed abseil
