#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

# Usage ./install_thirdparty.sh <install-path> <thirdparty-tags-file>
usage() {
    echo "Usage: $0 <install-path> <thirdparty-tags-file>"
    echo "  install-path: Path where third-party libraries will be installed"
    echo "  thirdparty-tags-file: Optional file containing tags for third-party libraries (default: opentelemetry-cpp/third_party_release)"
    exit 1
}

THRIDPARTY_BUILD_DIR=$HOME/third-party-build
THIRDPARTY_TAGS_FILE=''
SRC_DIR="$(pwd)"

if [ $# -eq 0 ] || [ $# -gt 2 ] || [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    usage
fi

if [ $# -ge 1 ]; then
    THIRDPARTY_INSTALL_DIR="$1"
fi

if [ $# -eq 2 ]; then
    THIRDPARTY_TAGS_FILE="$2"
fi

if [ ! -f "${SRC_DIR}/install/cmake/CMakeLists.txt" ]; then
    echo "Error: ${SRC_DIR}/install/cmake/CMakeLists.txt not found"
    echo "Please run this script from the root of the opentelemetry-cpp repository"
    usage
fi

mkdir -p "${THRIDPARTY_BUILD_DIR}"
cd "${THRIDPARTY_BUILD_DIR}"
rm -rf ./*

cmake -S "${SRC_DIR}"/install/cmake -B . \
   -DCMAKE_INSTALL_PREFIX="${THIRDPARTY_INSTALL_DIR}" \
   -DCMAKE_CXX_STANDARD="$CXX_STANDARD" \
   -DOTELCPP_THIRDPARTY_FILE="${THIRDPARTY_TAGS_FILE}" \
   -DOTELCPP_PROTO_PATH="${OTELCPP_PROTO_PATH}"

make -j"$(nproc)"

if [ "${THIRDPARTY_INSTALL_DIR}" = "/usr/local" ]; then
  ldconfig
fi
