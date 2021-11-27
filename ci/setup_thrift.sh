#!/bin/bash

set -e
export DEBIAN_FRONTEND=noninteractive
export THRIFT_VERSION=0.14.1

if ! type cmake > /dev/null; then
    #cmake not installed, exiting
    exit 1
fi
export BUILD_DIR=/tmp/
export INSTALL_DIR=/usr/local/

function thrift_ubuntu {
    apt install -y --no-install-recommends \
        libboost-all-dev \
        libevent-dev \
        libssl-dev \
        ninja-build

    pushd $BUILD_DIR
    wget https://github.com/apache/thrift/archive/refs/tags/v${THRIFT_VERSION}.tar.gz
    tar -zxvf v${THRIFT_VERSION}.tar.gz
    cd thrift-${THRIFT_VERSION}
    mkdir -p out
    pushd out
    cmake -G Ninja .. \
        -DBUILD_COMPILER=OFF \
        -DBUILD_CPP=ON \
        -DBUILD_LIBRARIES=ON \
        -DBUILD_NODEJS=OFF \
        -DBUILD_PYTHON=OFF \
        -DBUILD_JAVASCRIPT=OFF \
        -DBUILD_C_GLIB=OFF \
        -DBUILD_JAVA=OFF \
        -DBUILD_TESTING=OFF \
        -DBUILD_TUTORIALS=OFF \
        ..

    ninja -j $(nproc)
    ninja install
    popd
    popd
}

function thrift_mac {
    which -s brew
    if [[ $? != 0 ]] ; then
        # Install Homebrew
        ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    else
        # FIXME: do we always require the brew update??
        brew update
    fi
    brew install thrift
}

OS_NAME=`uname -a`
case "$OS_NAME" in
*Darwin*)
    thrift_mac ;;

*Linux*)
    thrift_ubuntu ;;
*)
    echo "WARNING: unsupported OS $OS_NAME. Skipping build tools installation." ;;
esac