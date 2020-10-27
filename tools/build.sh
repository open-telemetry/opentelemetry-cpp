#!/bin/bash

export PATH=/usr/local/bin:$PATH

DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
WORKSPACE_ROOT=$DIR/..
cd $WORKSPACE_ROOT
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
BUILD_TYPE="release"
else
BUILD_TYPE="debug"
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

# TODO: fix compiler-version reporting
if [ -f /usr/bin/gcc ]; then
echo "gcc   version: `gcc --version`"
COMPILER_NAME=gcc-`gcc -dumpversion`
fi

if [ -f /usr/bin/clang ]; then
echo "clang version: `clang --version`"
COMPILER_NAME=clang-`clang -dumpversion`
fi

# TODO: build and install Google Test from source if necessary

function build_configuration {
  BUILD_CONFIG=$1
  BUILD_OPTIONS=$2

  echo "Build configuration: $BUILD_CONFIG"
  cd $WORKSPACE_ROOT
  OUTDIR=out/$COMPILER_NAME/$BUILD_CONFIG
  mkdir -p $OUTDIR
  pushd $OUTDIR
  cmake $BUILD_OPTIONS $WORKSPACE_ROOT
  make
  popd
}

# Example
build_configuration nostd '-DWITH_MY_OPTION:BOOL=OFF'

