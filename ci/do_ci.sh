#!/bin/bash

set -e

[ -z "${SRC_DIR}" ] && export SRC_DIR="`pwd`"
[ -z "${BUILD_DIR}" ] && export BUILD_DIR=$HOME/build
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
  bazel build $BAZEL_OPTIONS -- //...
  bazel test $BAZEL_TEST_OPTIONS //...
  exit 0
elif [[ "$1" == "bazel.asan" ]]; then
  bazel test --config=asan $BAZEL_TEST_OPTIONS //...
  exit 0
elif [[ "$1" == "benchmark" ]]; then
  [ -z "${BENCHMARK_DIR}" ] && export BENCHMARK_DIR=$HOME/benchmark
  bazel build $BAZEL_OPTIONS -c opt -- \
    $(bazel query 'attr("tags", "benchmark_result", ...)')
  echo ""
  echo "Benchmark results in $BENCHMARK_DIR:"
  (
    cd bazel-bin
    find . -name \*_result.txt -exec bash -c \
      'echo "$@" && mkdir -p "$BENCHMARK_DIR/$(dirname "$@")" && \
       cp "$@" "$BENCHMARK_DIR/$@" && chmod +w "$BENCHMARK_DIR/$@"' _ {} \;
  )
  exit 0
elif [[ "$1" == "format" ]]; then
  tools/format.sh
  CHANGED="$(git ls-files --modified)"
  if [[ ! -z "$CHANGED" ]]; then
    echo "The following files have changes:"
    echo "$CHANGED"
    exit 1
  fi
  exit 0
fi

echo "Invalid do_ci.sh target, see ci/README.md for valid targets."
exit 1
