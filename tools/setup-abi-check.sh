#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

#
# This script installs latest ABI tools on Linux machine
#

export INSTALL_PREFIX=/usr/local

export PATH=${INSTALL_PREFIX}/bin:$PATH

rm -rf tmp-abi
mkdir tmp-abi
cd tmp-abi

echo "Installing libelf-dev ..."
echo ""

apt update
apt install -y --no-install-recommends libelf-dev

echo "Cloning vtable-dumper ..."
echo ""

git clone https://github.com/lvc/vtable-dumper.git

echo "Cloning abi-dumper ..."
echo ""

git clone https://github.com/lvc/abi-dumper.git

echo "Cloning abi-compliance-checker ..."
echo ""

git clone https://github.com/lvc/abi-compliance-checker.git

echo "Installing vtable-dumper ..."
echo ""

cd vtable-dumper
make install prefix=${INSTALL_PREFIX}
cd ..

echo "Installing abi-dumper ..."
echo ""

cd abi-dumper
make install prefix=${INSTALL_PREFIX}
cd ..

echo "Installing abi-compliance-checker ..."
echo ""

cd abi-compliance-checker
make install prefix=${INSTALL_PREFIX}
cd ..

rm -rf tmp-abi

which vtable-dumper
which abi-dumper
which abi-compliance-checker

vtable-dumper --version
abi-dumper --version
abi-compliance-checker --version

