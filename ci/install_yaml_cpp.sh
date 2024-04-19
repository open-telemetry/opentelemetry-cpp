#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -ex
export DEBIAN_FRONTEND=noninteractive
[ -z "${YAML_CPP_VERSION}" ] && export YAML_CPP_VERSION="0.8.0"

BUILD_DIR=/tmp/
INSTALL_DIR=/usr/local/
pushd $BUILD_DIR
git clone --depth=1 -b ${YAML_CPP_VERSION} https://github.com/jbeder/yaml-cpp.git
cd yaml-cpp
YAML_CPP_BUILD_OPTIONS=(
    "-DBUILD_TESTING=OFF"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_INSTALL_PREFIX=$INSTALL_DIR"
)

if [ ! -z "${CXX_STANDARD}" ]; then
    YAML_CPP_BUILD_OPTIONS=(${YAML_CPP_BUILD_OPTIONS[@]} "-DCMAKE_CXX_STANDARD=${CXX_STANDARD}")
fi

mkdir build && pushd build
cmake "${YAML_CPP_BUILD_OPTIONS[@]}" ..
make -j $(nproc)
make install
popd
popd
export PATH=${INSTALL_DIR}/bin:$PATH  # ensure to use the installed yaml-cpp
