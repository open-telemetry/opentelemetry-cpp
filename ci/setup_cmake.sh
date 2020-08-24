#!/bin/bash

set -e

apt update
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
