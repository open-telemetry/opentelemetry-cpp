#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -ex
export DEBIAN_FRONTEND=noninteractive

# See https://github.com/biojppm/rapidyaml/issues/468
# [ -z "${RYML_VERSION}" ] && export RYML_VERSION="v0.7.1"

# work around:
[ -z "${RYML_VERSION}" ] && export RYML_VERSION="v0.7.1-marc"

BUILD_DIR=/tmp/
INSTALL_DIR=/usr/local/
pushd $BUILD_DIR
# See https://github.com/biojppm/rapidyaml/issues/468
# git clone --recursive -b ${RYML_VERSION} https://github.com/biojppm/rapidyaml.git

# Work around
git clone --recursive -b ${RYML_VERSION} https://github.com/marcalff/rapidyaml.git

cd rapidyaml
RYML_BUILD_OPTIONS=(
    "-DBUILD_TESTING=OFF"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_INSTALL_PREFIX=$INSTALL_DIR"
)

if [ ! -z "${CXX_STANDARD}" ]; then
    RYML_BUILD_OPTIONS=(${RYML_BUILD_OPTIONS[@]} "-DCMAKE_CXX_STANDARD=${CXX_STANDARD}")
fi

mkdir build && pushd build
cmake "${RYML_BUILD_OPTIONS[@]}" ..
make -j $(nproc)
make install
popd
popd
export PATH=${INSTALL_DIR}/bin:$PATH  # ensure to use the installed ryml
