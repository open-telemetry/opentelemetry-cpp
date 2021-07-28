#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

function install_prometheus_cpp_client
{
  pushd third_party/prometheus-cpp
  git submodule update --recursive --init
  [[ -d _build ]] && rm -rf ./_build
  mkdir _build && cd _build
  cmake .. -DBUILD_SHARED_LIBS=ON -DUSE_THIRDPARTY_LIBRARIES=ON
  make -j 4
  sudo make install
  popd
}

[ -z "${SRC_DIR}" ] && export SRC_DIR="`pwd`"
[ -z "${BUILD_DIR}" ] && export BUILD_DIR=$HOME/build
mkdir -p "${BUILD_DIR}"
[ -z "${PLUGIN_DIR}" ] && export PLUGIN_DIR=$HOME/plugin
mkdir -p "${PLUGIN_DIR}"

BAZEL_OPTIONS="--copt=-DENABLE_METRICS_PREVIEW --copt=-DENABLE_LOGS_PREVIEW"
BAZEL_TEST_OPTIONS="$BAZEL_OPTIONS --test_output=errors"
BAZEL_STARTUP_OPTIONS="--output_user_root=$HOME/.cache/bazel"

export CTEST_OUTPUT_ON_FAILURE=1

if [[ "$1" == "cmake.test" ]]; then
  install_prometheus_cpp_client
  cd "${BUILD_DIR}"
  rm -rf *
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DWITH_PROMETHEUS=ON \
        -DWITH_ZIPKIN=ON \
        -DWITH_JAEGER=ON \
        -DWITH_ELASTICSEARCH=ON \
        -DWITH_METRICS_PREVIEW=ON \
        -DWITH_LOGS_PREVIEW=ON \
        -DCMAKE_CXX_FLAGS="-Werror" \
        "${SRC_DIR}"
  make
  make test
  exit 0
elif [[ "$1" == "cmake.abseil.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DWITH_METRICS_PREVIEW=ON \
        -DWITH_LOGS_PREVIEW=ON \
        -DCMAKE_CXX_FLAGS="-Werror" \
        -DWITH_ABSEIL=ON \
        "${SRC_DIR}"
  make
  make test
  exit 0
elif [[ "$1" == "cmake.c++20.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DCMAKE_CXX_FLAGS="-Werror" \
        -DCMAKE_CXX_STANDARD=20 \
        "${SRC_DIR}"
  make
  make test
  exit 0
elif [[ "$1" == "cmake.c++20.stl.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DWITH_METRICS_PREVIEW=ON \
        -DWITH_LOGS_PREVIEW=ON \
        -DCMAKE_CXX_FLAGS="-Werror" \
        -DWITH_STL=ON \
        "${SRC_DIR}"
  make
  make test
  exit 0
elif [[ "$1" == "cmake.legacy.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  export BUILD_ROOT="${BUILD_DIR}"
  ${SRC_DIR}/tools/build-gtest.sh
  ${SRC_DIR}/tools/build-benchmark.sh
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DCMAKE_CXX_FLAGS="-Werror" \
        -DCMAKE_CXX_STANDARD=11 \
        "${SRC_DIR}"
  make
  make test
  exit 0
elif [[ "$1" == "cmake.legacy.exporter.otprotocol.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DCMAKE_CXX_STANDARD=11 \
        -DWITH_OTLP=ON \
        "${SRC_DIR}"
  grpc_cpp_plugin=`which grpc_cpp_plugin`
  proto_make_file="CMakeFiles/opentelemetry_proto.dir/build.make"
  sed -i "s~gRPC_CPP_PLUGIN_EXECUTABLE-NOTFOUND~$grpc_cpp_plugin~" ${proto_make_file} #fixme
  make -j $(nproc)
  cd exporters/otlp && make test
  exit 0
elif [[ "$1" == "cmake.exporter.otprotocol.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DWITH_OTLP=ON \
        "${SRC_DIR}"
  grpc_cpp_plugin=`which grpc_cpp_plugin`
  proto_make_file="CMakeFiles/opentelemetry_proto.dir/build.make"
  sed -i "s~gRPC_CPP_PLUGIN_EXECUTABLE-NOTFOUND~$grpc_cpp_plugin~" ${proto_make_file} #fixme
  make -j $(nproc)
  cd exporters/otlp && make test
  exit 0
elif [[ "$1" == "cmake.test_example_plugin" ]]; then
  # Build the plugin
  cd "${BUILD_DIR}"
  rm -rf *
  cat <<EOF > export.map
{
  global:
    OpenTelemetryMakeFactoryImpl;
  local: *;
};
EOF

  LINKER_FLAGS="\
    -static-libstdc++ \
    -static-libgcc \
    -Wl,--version-script=${PWD}/export.map \
  "
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DCMAKE_CXX_FLAGS="-Werror" \
        -DCMAKE_EXE_LINKER_FLAGS="$LINKER_FLAGS" \
        -DCMAKE_SHARED_LINKER_FLAGS="$LINKER_FLAGS" \
        "${SRC_DIR}"
  make example_plugin
  cp examples/plugin/plugin/libexample_plugin.so ${PLUGIN_DIR}

  # Verify we can load the plugin
  cd "${BUILD_DIR}"
  rm -rf *
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DCMAKE_CXX_FLAGS="-Werror" \
        "${SRC_DIR}"
  make load_plugin_example
  examples/plugin/load/load_plugin_example ${PLUGIN_DIR}/libexample_plugin.so /dev/null
  exit 0
elif [[ "$1" == "bazel.test" ]]; then
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS //...
  bazel $BAZEL_STARTUP_OPTIONS test $BAZEL_TEST_OPTIONS //...
  exit 0
elif [[ "$1" == "bazel.legacy.test" ]]; then
  # we uses C++ future and async() function to test the Prometheus Exporter functionality,
  # that make this test always fail. ignore Prometheus exporter here.
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS -- //... -//exporters/otlp/... -//exporters/prometheus/...
  bazel $BAZEL_STARTUP_OPTIONS test $BAZEL_TEST_OPTIONS -- //... -//exporters/otlp/... -//exporters/prometheus/...
  exit 0
elif [[ "$1" == "bazel.noexcept" ]]; then
  # there are some exceptions and error handling code from the Prometheus Client
  # that make this test always fail. ignore Prometheus exporter in the noexcept here.
  bazel $BAZEL_STARTUP_OPTIONS build --copt=-fno-exceptions $BAZEL_OPTIONS -- //... -//exporters/prometheus/...
  bazel $BAZEL_STARTUP_OPTIONS test --copt=-fno-exceptions $BAZEL_TEST_OPTIONS -- //... -//exporters/prometheus/...
  exit 0
elif [[ "$1" == "bazel.asan" ]]; then
  bazel $BAZEL_STARTUP_OPTIONS test --config=asan $BAZEL_TEST_OPTIONS //...
  exit 0
elif [[ "$1" == "bazel.tsan" ]]; then
  bazel $BAZEL_STARTUP_OPTIONS test --config=tsan $BAZEL_TEST_OPTIONS //...
  exit 0
elif [[ "$1" == "bazel.valgrind" ]]; then
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS //...
  bazel $BAZEL_STARTUP_OPTIONS test --run_under="/usr/bin/valgrind --leak-check=full --error-exitcode=1 --suppressions=\"${SRC_DIR}/ci/valgrind-suppressions\"" $BAZEL_TEST_OPTIONS //...
  exit 0
elif [[ "$1" == "benchmark" ]]; then
  [ -z "${BENCHMARK_DIR}" ] && export BENCHMARK_DIR=$HOME/benchmark
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS -c opt -- \
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
    git diff
    exit 1
  fi
  exit 0
elif [[ "$1" == "code.coverage" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DCMAKE_CXX_FLAGS="-Werror --coverage" \
        "${SRC_DIR}"
  make
  make test
  lcov --directory $PWD --capture --output-file coverage.info
  # removing test http server coverage from the total coverage. We don't use this server completely.
  lcov --remove coverage.info '*/ext/http/server/*'> tmp_coverage.info 2>/dev/null
  cp tmp_coverage.info coverage.info
  exit 0
fi

echo "Invalid do_ci.sh target, see ci/README.md for valid targets."
exit 1
