#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e
export DEBIAN_FRONTEND=noninteractive
old_grpc_version='v1.33.2'
new_grpc_version='v1.49.2'
gcc_version_for_new_grpc='5.1'
std_version='14'
install_grpc_version=${new_grpc_version}
install_dir='/usr/local/'
build_shared_libs=''
build_internal_abseil_cpp=1
GRPC_BUILD_OPTIONS=()
usage() { echo "Usage: $0 [-v <gcc-version>] [-i <install_dir>"] 1>&2; exit 1;}

while getopts ":v:i:p:r:s:TH" o; do
    case "${o}" in
        v)
            gcc_version=${OPTARG}
            ;;
        i)
            install_dir=${OPTARG}
            ;;
        p)
            if [ "${OPTARG}" == "protobuf" ]; then
                GRPC_BUILD_OPTIONS=(${GRPC_BUILD_OPTIONS[@]} "-DgRPC_PROTOBUF_PROVIDER=package")
            elif [ "${OPTARG}" == "abseil-cpp" ]; then
                GRPC_BUILD_OPTIONS=(${GRPC_BUILD_OPTIONS[@]} "-DgRPC_ABSL_PROVIDER=package")
                build_internal_abseil_cpp=0
            fi
            ;;
        r)
            install_grpc_version=${OPTARG}
            ;;
        s)
            std_version=${OPTARG}
            ;;
        T)
            build_shared_libs="OFF"
            ;;
        H)
            build_shared_libs="ON"
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
    std_version='11'
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

if [[ $build_internal_abseil_cpp -ne 0 ]]; then
    mkdir -p "third_party/abseil-cpp/build" && pushd "third_party/abseil-cpp/build"
    set -x

    ABSEIL_CPP_BUILD_OPTIONS=(
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE
        -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
    )
    if [ ! -z "$build_shared_libs" ]; then
        ABSEIL_CPP_BUILD_OPTIONS=(${ABSEIL_CPP_BUILD_OPTIONS[@]} "-DBUILD_SHARED_LIBS=$build_shared_libs")
    fi
    cmake  ${ABSEIL_CPP_BUILD_OPTIONS[@]} ..
    cmake --build . -j${nproc} --target install && popd
fi
mkdir -p build && pushd build

GRPC_BUILD_OPTIONS=(
    ${GRPC_BUILD_OPTIONS[@]}
    -DgRPC_INSTALL=ON
    -DCMAKE_CXX_STANDARD=${std_version}
    -DgRPC_BUILD_TESTS=OFF
    -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
    -DCMAKE_PREFIX_PATH=$INSTALL_DIR
)
if [ ! -z "$build_shared_libs" ]; then
    GRPC_BUILD_OPTIONS=(${GRPC_BUILD_OPTIONS[@]} "-DBUILD_SHARED_LIBS=$build_shared_libs")
fi

cmake ${GRPC_BUILD_OPTIONS[@]} ..
cmake --build . -j$(nproc)
cmake --install .
popd
popd

export PATH=${INSTALL_DIR}/bin:$PATH  # ensure to use the installed grpc
