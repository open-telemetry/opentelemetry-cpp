#!/bin/bash

# Copyright The OpenTelemetry Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

export DEBIAN_FRONTEND=noninteractive
apt-get update

export CMAKE_VERSION=3.15.2
export GOOGLETEST_VERSION=1.10.0

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
    wget https://github.com/google/googletest/archive/release-${GOOGLETEST_VERSION}.tar.gz
    tar -xf release-${GOOGLETEST_VERSION}.tar.gz
    cd googletest-release-${GOOGLETEST_VERSION}/
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
