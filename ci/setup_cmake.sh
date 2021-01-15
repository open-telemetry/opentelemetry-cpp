#!/bin/bash

set -e

export DEBIAN_FRONTEND=noninteractive
apt-get update

export CMAKE_VERSION=3.15.2

pushd /tmp
wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh
chmod +x cmake-${CMAKE_VERSION}-Linux-x86_64.sh
./cmake-${CMAKE_VERSION}-Linux-x86_64.sh --prefix=/usr/local --skip-license
popd

set +e
echo		\
                libbenchmark-dev \
                libgtest-dev \
                zlib1g-dev \
                sudo \
                libcurl4-openssl-dev \
                nlohmann-json-dev \
                nlohmann-json3 \
                nlohmann-json3-dev | xargs -n 1 apt-get install --ignore-missing --no-install-recommends --no-install-suggests -y
set -e

# Follows these instructions for setting up gtest
# https://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/
pushd /usr/src/gtest
cmake CMakeLists.txt
make
cp *.a /usr/lib || cp lib/*.a /usr/lib
popd
