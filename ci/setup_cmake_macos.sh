#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

CMAKE_VERSION="${CMAKE_VERSION:-3.31.6}"
CMAKE_PKG="cmake-${CMAKE_VERSION}-macos-universal"
CMAKE_TAR="${CMAKE_PKG}.tar.gz"
CMAKE_URL="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/${CMAKE_TAR}"

INSTALL_DIR="/opt/cmake"
BIN_DIR="${INSTALL_DIR}/cmake/CMake.app/Contents/bin"

echo "Installing CMake version ${CMAKE_VERSION}..."

if brew list cmake >/dev/null 2>&1; then
  echo "Removing existing Homebrew-installed CMake..."
  brew uninstall cmake
fi

if ! command -v wget >/dev/null 2>&1; then
  echo "wget not found. Installing wget via Homebrew..."
  brew install wget
fi

wget -q "${CMAKE_URL}"
tar -xzf "${CMAKE_TAR}"

sudo mkdir -p "${INSTALL_DIR}"
sudo mv "${CMAKE_PKG}" "${INSTALL_DIR}/cmake"

for file in "${BIN_DIR}"/*; do
  sudo ln -sf "${file}" "/usr/local/bin/$(basename "${file}")"
done

rm -f "${CMAKE_TAR}"

cmake --version
