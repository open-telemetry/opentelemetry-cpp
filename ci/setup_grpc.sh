#!/bin/bash

set -e
export DEBIAN_FRONTEND=noninteractive

if ! type cmake > /dev/null; then
    #cmake not installed, exiting
    exit 1
fi
export BUILD_DIR=/tmp/
export INSTALL_DIR=/usr/local/
pushd $BUILD_DIR
git clone --recurse-submodules -b v1.34.0 https://github.com/grpc/grpc
cd grpc
mkdir -p cmake/build
pushd cmake/build
cmake -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
    ../..
make -j 6
make install
popd
popd
export PATH=${INSTALL_DIR}/bin:$PATH  # nsure to use the installed grpc
