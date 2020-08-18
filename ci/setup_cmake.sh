#!/bin/bash

set -e

apt-get install --no-install-recommends --no-install-suggests -y \
                cmake \
                libbenchmark-dev \
                libgtest-dev \
                libcurl4-openssl-dev

# Follows these instructions for setting up gtest
# https://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/
pushd /usr/src/gtest
cmake CMakeLists.txt
make
cp *.a /usr/lib || cp lib/*.a /usr/lib
popd

# Prometheus client
git clone https://github.com/jupp0r/prometheus-cpp
cd prometheus-cpp
git checkout v0.9.0
git submodule init
git submodule update
mkdir _build && cd _build
cmake .. -DBUILD_SHARED_LIBS=ON
make && make install
