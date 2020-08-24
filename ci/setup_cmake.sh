#!/bin/bash

set -e

export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install --no-install-recommends --no-install-suggests -y \
                cmake \
                libbenchmark-dev \
                libgtest-dev \
                zlib1g-dev \
                sudo \
                libcurl4-openssl-dev

# Follows these instructions for setting up gtest
# https://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/
pushd /usr/src/gtest
cmake CMakeLists.txt
make
cp *.a /usr/lib || cp lib/*.a /usr/lib
popd
