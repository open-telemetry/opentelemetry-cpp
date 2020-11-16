#!/bin/bash
export PATH=/usr/local/bin:$PATH
DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
WORKSPACE_ROOT=$DIR/..

export VCPKG_ROOT=$WORKSPACE_ROOT/tools/vcpkg
export PATH=$VCPKG_ROOT:$PATH

if [[ ! -f $DIR/vcpkg/vcpkg ]] ; then
  pushd $DIR/vcpkg
  ./bootstrap-vcpkg.sh
  popd
fi

vcpkg install gtest
vcpkg install benchmark
vcpkg install ms-gsl
vcpkg install nlohmann-json
vcpkg install abseil
vcpkg install protobuf

cd $WORKSPACE_ROOT
export USE_VCPKG=1
./tools/build.sh ${1-nostd} ${2--DCMAKE_TOOLCHAIN_FILE=$WORKSPACE_ROOT/tools/vcpkg/scripts/buildsystems/vcpkg.cmake}
