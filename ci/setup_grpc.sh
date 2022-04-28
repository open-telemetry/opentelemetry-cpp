#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e
export DEBIAN_FRONTEND=noninteractive
old_grpc_version='v1.33.2'
new_grpc_version='v1.43.2'
gcc_version_for_new_grpc='5.1'
install_grpc_version=${new_grpc_version}
grpc_version='v1.39.0'
install_dir='/usr/local/'
usage() { echo "Usage: $0 [-v <gcc-version>] [-i <install_dir>"] 1>&2; exit 1;}

while getopts ":v:i:" o; do
    case "${o}" in
        v)
            gcc_version=${OPTARG}
            ;;
        i)
            install_dir=${OPTARG}
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
export INSTALL_DIR=${install_dir}
pushd $BUILD_DIR
echo "installing grpc version: ${install_grpc_version}"
git clone --depth=1 -b ${install_grpc_version} https://github.com/grpc/grpc
pushd grpc
git submodule init
git submodule update --depth 1
mkdir -p "third_party/abseil-cpp/build" && pushd "third_party/abseil-cpp/build"
cmake -DCMAKE_BUILD_TYPE=Release  \
      -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR ..
make -j${nproc} install && popd
mkdir -p build && pushd build
cmake -DgRPC_INSTALL=ON \
    -DCMAKE_CXX_STANDARD=11 \
    -DgRPC_BUILD_TESTS=OFF \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
    -DCMAKE_PREFIX_PATH=$INSTALL_DIR \
    ..
make -j $(nproc)
make install
popd
popd

export PATH=${INSTALL_DIR}/bin:$PATH  # ensure to use the installed grpc
