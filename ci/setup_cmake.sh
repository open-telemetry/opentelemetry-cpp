#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

export DEBIAN_FRONTEND=noninteractive
apt-get update

if [ "x$CMAKE_VERSION" = "x" ]; then
  export CMAKE_VERSION=3.15.2
fi

# This variable not set on CI pipeline for only legacy environment(GCC 4.8).
# With 1.13.0 version, C++14 must be set which does not supported by legacy environment anymore.
# Also with this version, release version path needs to be adapted.
if [ "x$GOOGLETEST_VERSION" = "x" ]; then
  export GOOGLETEST_VERSION=1.10.0
  GOOGLETEST_VERSION_PATH="release-${GOOGLETEST_VERSION}"
  GOOGLETEST_FOLDER_PATH="googletest-release-${GOOGLETEST_VERSION}"
else
  GOOGLETEST_VERSION_PATH="v${GOOGLETEST_VERSION}"
  GOOGLETEST_FOLDER_PATH="googletest-${GOOGLETEST_VERSION}"
fi

cmake_install() {
    tmp_dir=$(mktemp -d)
    pushd $tmp_dir
    wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh
    chmod +x cmake-${CMAKE_VERSION}-Linux-x86_64.sh
    ./cmake-${CMAKE_VERSION}-Linux-x86_64.sh --prefix=/usr/local --skip-license
    rm cmake-${CMAKE_VERSION}-Linux-x86_64.sh
    popd
}

googletest_install() {
    # Follows these instructions
    # https://gist.github.com/dlime/313f74fd23e4267c4a915086b84c7d3d
    tmp_dir=$(mktemp -d)
    pushd $tmp_dir
    wget https://github.com/google/googletest/archive/${GOOGLETEST_VERSION_PATH}.tar.gz
    tar -xf ${GOOGLETEST_VERSION_PATH}.tar.gz
    cd ${GOOGLETEST_FOLDER_PATH}/
    mkdir build && cd build
    cmake .. -DBUILD_SHARED_LIBS=ON -DINSTALL_GTEST=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr
    make -j $(nproc)
    make install
    ldconfig
    popd
}

cmake_install

set +e
echo		\
                libbenchmark-dev \
                zlib1g-dev \
                sudo \
                libcurl4-openssl-dev \
                nlohmann-json-dev \
                nlohmann-json3 \
                nlohmann-json3-dev | xargs -n 1 apt-get install --ignore-missing --no-install-recommends --no-install-suggests -y
set -e

googletest_install
