#!/bin/bash

set -e

export DEBIAN_FRONTEND=noninteractive
apt-get update

set +e
echo		\
                cmake \
                libbenchmark-dev \
                libgtest-dev \
                zlib1g-dev \
                sudo \
                libcurl4-openssl-dev \
                nlohmann-json-dev \
                nlohmann-json3-dev | xargs -n 1 apt-get install --ignore-missing --no-install-recommends --no-install-suggests -y
set -e

# Follows these instructions for setting up gtest
# https://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/
pushd /usr/src/gtest
cmake CMakeLists.txt
make
cp *.a /usr/lib || cp lib/*.a /usr/lib
popd
