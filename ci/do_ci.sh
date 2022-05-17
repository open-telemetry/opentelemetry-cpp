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

function run_benchmarks
{
  docker run -d --rm -it -p 4317:4317 -p 4318:4318 -v \
    $(pwd)/examples/otlp:/cfg otel/opentelemetry-collector:0.38.0 \
    --config=/cfg/opentelemetry-collector-config/config.dev.yaml

  [ -z "${BENCHMARK_DIR}" ] && export BENCHMARK_DIR=$HOME/benchmark
  mkdir -p $BENCHMARK_DIR
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS -c opt -- \
    $(bazel query 'attr("tags", "benchmark_result", ...)')
  echo ""
  echo "Benchmark results in $BENCHMARK_DIR:"
  (
    cd bazel-bin
    find . -name \*_result.json -exec bash -c \
      'echo "$@" && mkdir -p "$BENCHMARK_DIR/$(dirname "$@")" && \
       cp "$@" "$BENCHMARK_DIR/$@" && chmod +w "$BENCHMARK_DIR/$@"' _ {} \;
  )

  # collect benchmark results into one array
  pushd $BENCHMARK_DIR
  components=(api sdk exporters)
  for component in "${components[@]}"
  do
    out=$component-benchmark_result.json
    find ./$component -type f -name "*_result.json" -exec cat {} \; > $component_tmp_bench.json
    cat $component_tmp_bench.json | docker run -i --rm itchyny/gojq:0.12.6 -s \
      '.[0].benchmarks = ([.[].benchmarks] | add) |
      if .[0].benchmarks == null then null else .[0] end' > $BENCHMARK_DIR/$out
  done

  mv *benchmark_result.json ${SRC_DIR}
  popd
  docker kill $(docker ps -q)
}

[ -z "${SRC_DIR}" ] && export SRC_DIR="`pwd`"
[ -z "${BUILD_DIR}" ] && export BUILD_DIR=$HOME/build
mkdir -p "${BUILD_DIR}"
[ -z "${PLUGIN_DIR}" ] && export PLUGIN_DIR=$HOME/plugin
mkdir -p "${PLUGIN_DIR}"

BAZEL_OPTIONS="--copt=-DENABLE_LOGS_PREVIEW --copt=-DENABLE_TEST"
# Previous legacy metrics use virtual drive, which can not be used without RTTI
if [[ "$1" != "bazel.nortti" ]]; then
  BAZEL_OPTIONS="$BAZEL_OPTIONS --copt=-DENABLE_METRICS_PREVIEW"
fi
BAZEL_TEST_OPTIONS="$BAZEL_OPTIONS --test_output=errors"

# https://github.com/bazelbuild/bazel/issues/4341
BAZEL_MACOS_OPTIONS="$BAZEL_OPTIONS --features=-supports_dynamic_linker --build_tag_filters=-jaeger"
BAZEL_MACOS_TEST_OPTIONS="$BAZEL_MACOS_OPTIONS --test_output=errors"

BAZEL_STARTUP_OPTIONS="--output_user_root=$HOME/.cache/bazel"

export CTEST_OUTPUT_ON_FAILURE=1

if [[ "$1" == "cmake.test" ]]; then
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
  export BUILD_ROOT="${BUILD_DIR}"
  ${SRC_DIR}/tools/build-gtest.sh
  ${SRC_DIR}/tools/build-benchmark.sh
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
elif [[ "$1" == "bazel.with_abseil" ]]; then
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS --//api:with_abseil=true //...
  bazel $BAZEL_STARTUP_OPTIONS test $BAZEL_TEST_OPTIONS --//api:with_abseil=true //...
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
elif [[ "$1" == "bazel.benchmark" ]]; then
  run_benchmarks
  exit 0
elif [[ "$1" == "bazel.macos.test" ]]; then
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_MACOS_OPTIONS -- //... -//exporters/jaeger/...
  bazel $BAZEL_STARTUP_OPTIONS test $BAZEL_MACOS_TEST_OPTIONS -- //... -//exporters/jaeger/...
  exit 0
elif [[ "$1" == "bazel.legacy.test" ]]; then
  # we uses C++ future and async() function to test the Prometheus Exporter functionality,
  # that make this test always fail. ignore Prometheus exporter here.
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS -- //... -//exporters/otlp/... -//exporters/prometheus/...
  bazel $BAZEL_STARTUP_OPTIONS test $BAZEL_TEST_OPTIONS -- //... -//exporters/otlp/... -//exporters/prometheus/...
  exit 0
elif [[ "$1" == "bazel.noexcept" ]]; then
  # there are some exceptions and error handling code from the Prometheus and Jaeger Clients
  # that make this test always fail. ignore Prometheus and Jaeger exporters in the noexcept here.
  bazel $BAZEL_STARTUP_OPTIONS build --copt=-fno-exceptions --build_tag_filters=-jaeger $BAZEL_OPTIONS -- //... -//exporters/prometheus/... -//exporters/jaeger/... -//examples/prometheus/...
  bazel $BAZEL_STARTUP_OPTIONS test --copt=-fno-exceptions --build_tag_filters=-jaeger $BAZEL_TEST_OPTIONS -- //... -//exporters/prometheus/... -//exporters/jaeger/... -//examples/prometheus/...
  exit 0
elif [[ "$1" == "bazel.nortti" ]]; then
  # there are some exceptions and error handling code from the Prometheus and Jaeger Clients
  # that make this test always fail. ignore Prometheus and Jaeger exporters in the noexcept here.
  bazel $BAZEL_STARTUP_OPTIONS build --cxxopt=-fno-rtti --build_tag_filters=-jaeger $BAZEL_OPTIONS -- //... -//exporters/prometheus/... -//exporters/jaeger/...
  bazel $BAZEL_STARTUP_OPTIONS test --cxxopt=-fno-rtti --build_tag_filters=-jaeger $BAZEL_TEST_OPTIONS -- //... -//exporters/prometheus/... -//exporters/jaeger/...
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
    find . -name \*_result.json -exec bash -c \
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
elif [[ "$1" == "third_party.tags" ]]; then
  echo "gRPC=v1.43.2" > third_party_release
  echo "thrift=0.14.1" >> third_party_release
  echo "abseil=20210324.0" >> third_party_release
  git submodule foreach --quiet 'echo "$name=$(git describe --tags HEAD)"' | sed 's:.*/::' >> third_party_release
  exit 0
fi

echo "Invalid do_ci.sh target, see ci/README.md for valid targets."
exit 1
