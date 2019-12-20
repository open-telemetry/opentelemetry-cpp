#!/bin/bash

set -e

[ -z "${SRC_DIR}" ] && export SRC_DIR="`pwd`"
[ -z "${BUILD_DIR}" ] && export BUILD_DIR=/build
mkdir -p "${BUILD_DIR}"

BAZEL_OPTIONS=""
BAZEL_TEST_OPTIONS="$BAZEL_OPTIONS --test_output=errors"

if [[ "$1" == "cmake.test" ]]; then
  cd "${BUILD_DIR}"
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DCMAKE_CXX_FLAGS="-Werror" \
        "${SRC_DIR}"
  make
  make test
  exit 0
elif [[ "$1" == "bazel.test" ]]; then
  bazel build -c dbg $BAZEL_OPTIONS -- //...
  bazel test -c dbg $BAZEL_TEST_OPTIONS //...
  exit 0
else
  echo "Invalid do_ci.sh target, see ci/README.md for valid targets."
  exit 1
fi
