#!/bin/bash

export PATH=/usr/local/bin:$PATH

DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $DIR/..
echo "Current directory is `pwd`"

export NOROOT=$NOROOT

if [ "$1" == "clean" ]; then
 rm -f CMakeCache.txt *.cmake
 rm -rf out
 rm -rf .buildtools
# make clean
fi

if [ "$1" == "noroot" ] || [ "$2" == "noroot" ]; then
export NOROOT=true
fi

if [ "$1" == "release" ] || [ "$2" == "release" ]; then
BUILD_TYPE="Release"
else
BUILD_TYPE="Debug"
fi

# Set target MacOS minver
export MACOSX_DEPLOYMENT_TARGET=10.10

# Install build tools
FILE=.buildtools
OS_NAME=`uname -a`
if [ ! -f $FILE ]; then
case "$OS_NAME" in
 *Darwin*) tools/setup-buildtools-mac.sh ;;
 *Linux*)  [[ -z "$NOROOT" ]] && sudo tools/setup-buildtools.sh || echo "No root: skipping build tools installation." ;;
 *)        echo "WARNING: unsupported OS $OS_NAME , skipping build tools installation.."
esac
# Assume that the build tools have been successfully installed
echo > $FILE
fi

if [ -f /usr/bin/gcc ]; then
echo "gcc   version: `gcc --version`"
fi

if [ -f /usr/bin/clang ]; then
echo "clang version: `clang --version`"
fi

# Install Google Test
# tools/build-gtest.sh

#
# Do the build
#
mkdir -p out
cd out
cmake ..
make
