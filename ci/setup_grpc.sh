#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e
export DEBIAN_FRONTEND=noninteractive
old_grpc_version='v1.34.0'
new_grpc_version='v1.39.0'
gcc_version_for_new_grpc='5.1'
install_grpc_version=${new_grpc_version}
grpc_version='v1.39.0'
usage() { echo "Usage: $0 -v <gcc-version>" 1>&2; exit 1; }

while getopts ":v:" o; do
    case "${o}" in
        v)
            gcc_version=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
if [ -z "${gcc_version}" ]; then
    gcc_version=`gcc --version | awk '/gcc/  {print $NF}'`
fi
if [[ "${gcc_version}" < "${gcc_version_for_new_grpc}" ]]; then
    echo "less"
    install_grpc_version=${old_grpc_version}
fi
if ! type cmake > /dev/null; then
    #cmake not installed, exiting
    exit 1
fi
export BUILD_DIR=/tmp/
export INSTALL_DIR=/usr/local/
pushd $BUILD_DIR
echo "installing grpc version: ${install_grpc_version}"
git clone --depth=1 -b ${install_grpc_version} https://github.com/grpc/grpc
cd grpc
git submodule init
git submodule update --depth 1
mkdir -p cmake/build
pushd cmake/build
cmake -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
    ../..
make -j $(nproc)
make install
popd
popd
export PATH=${INSTALL_DIR}/bin:$PATH  # ensure to use the installed grpc
