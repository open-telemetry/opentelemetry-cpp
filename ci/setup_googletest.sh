#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

export DEBIAN_FRONTEND=noninteractive
apt-get update

if [ -z "${GOOGLETEST_VERSION}" ]; then
  # Version by default. Requires C++14.
  export GOOGLETEST_VERSION=1.14.0
fi

INSTALL_DIR=/usr/local/

GOOGLETEST_BUILD_OPTIONS=(
    "-DBUILD_SHARED_LIBS=ON"
    "-DBUILD_GMOCK=ON"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DINSTALL_GTEST=ON"
    "-DCMAKE_INSTALL_PREFIX=$INSTALL_DIR"
)

if [ ! -z "${CXX_STANDARD}" ]; then
    GOOGLETEST_BUILD_OPTIONS+=("-DCMAKE_CXX_STANDARD=${CXX_STANDARD}")
    GOOGLETEST_BUILD_OPTIONS+=("-DCMAKE_CXX_STANDARD_REQUIRED=ON")
    GOOGLETEST_BUILD_OPTIONS+=("-DCMAKE_CXX_EXTENSIONS=OFF")
fi

if [ ! -z "${GTEST_HAS_ABSL}" ]; then
    GOOGLETEST_BUILD_OPTIONS+=("-DGTEST_HAS_ABSL=${GTEST_HAS_ABSL}")
fi

OLD_GOOGLETEST_VERSION_REGEXP="^1\.([0-9]|10|11|12)(\..*)?$"

if [[ ${GOOGLETEST_VERSION} =~ ${OLD_GOOGLETEST_VERSION_REGEXP} ]]; then
  # Old (up to 1.12.x included) download URL format.
  GOOGLETEST_VERSION_PATH="release-${GOOGLETEST_VERSION}"
  GOOGLETEST_FOLDER_PATH="googletest-release-${GOOGLETEST_VERSION}"
else
  # New (since 1.13.0) download URL format.
  GOOGLETEST_VERSION_PATH="v${GOOGLETEST_VERSION}"
  GOOGLETEST_FOLDER_PATH="googletest-${GOOGLETEST_VERSION}"
fi

googletest_install()
{
  tmp_dir=$(mktemp -d)
  pushd $tmp_dir
  wget https://github.com/google/googletest/archive/${GOOGLETEST_VERSION_PATH}.tar.gz
  tar -xf ${GOOGLETEST_VERSION_PATH}.tar.gz
  cd ${GOOGLETEST_FOLDER_PATH}/
  mkdir build && cd build

  echo "Installing googletest version: ${GOOGLETEST_VERSION}..."
  echo "Using build options:" "${GOOGLETEST_BUILD_OPTIONS[@]}"

  cmake "${GOOGLETEST_BUILD_OPTIONS[@]}" ..
  make -j $(nproc)
  make install
  ldconfig
  popd
}

googletest_install
