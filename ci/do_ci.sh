#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -eo pipefail

function run_benchmarks
{
  docker run -d --rm -it -p 4317:4317 -p 4318:4318 -v \
    $(pwd)/examples/otlp:/cfg otel/opentelemetry-collector:0.109.0 \
    --config=/cfg/opentelemetry-collector-config/config.dev.yaml

  [ -z "${BENCHMARK_DIR}" ] && export BENCHMARK_DIR=$HOME/benchmark
  mkdir -p $BENCHMARK_DIR
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS_ASYNC -c opt -- \
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
    # Print each result in CI logs, so it can be inspected.
    echo "BENCHMARK result (begin)"
    cat $component_tmp_bench.json
    echo "BENCHMARK result (end)"
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
[ -z "${INSTALL_TEST_DIR}" ] && export INSTALL_TEST_DIR=$HOME/install_test
mkdir -p "${INSTALL_TEST_DIR}"

export BAZEL_CXXOPTS="-std=c++17"

# Work around for https://github.com/actions/runner-images/issues/13564
export USE_BAZEL_VERSION="8.5.0"

BAZEL_OPTIONS_DEFAULT="--copt=-DENABLE_METRICS_EXEMPLAR_PREVIEW --//exporters/otlp:with_otlp_grpc_credential_preview=true"
BAZEL_OPTIONS="$BAZEL_OPTIONS_DEFAULT"

BAZEL_TEST_OPTIONS="$BAZEL_OPTIONS --test_output=errors"

BAZEL_OPTIONS_ASYNC="$BAZEL_OPTIONS --copt=-DENABLE_ASYNC_EXPORT"
BAZEL_TEST_OPTIONS_ASYNC="$BAZEL_OPTIONS_ASYNC --test_output=errors"

# https://github.com/bazelbuild/bazel/issues/4341
BAZEL_MACOS_OPTIONS="$BAZEL_OPTIONS_ASYNC --features=-supports_dynamic_linker"
BAZEL_MACOS_TEST_OPTIONS="$BAZEL_MACOS_OPTIONS --test_output=errors"

BAZEL_STARTUP_OPTIONS="--output_user_root=$HOME/.cache/bazel"

if [[ "${BUILD_TYPE}" =~ ^(Debug|Release|RelWithDebInfo|MinSizeRel)$ ]]; then
  CMAKE_OPTIONS=(-DCMAKE_BUILD_TYPE=${BUILD_TYPE})
else
  CMAKE_OPTIONS=(-DCMAKE_BUILD_TYPE=Debug)
fi

if [ -n "${CXX_STANDARD}" ]; then
  CMAKE_OPTIONS+=("-DCMAKE_CXX_STANDARD=${CXX_STANDARD}")
else
  CMAKE_OPTIONS+=("-DCMAKE_CXX_STANDARD=17")
fi

CMAKE_OPTIONS+=("-DCMAKE_CXX_STANDARD_REQUIRED=ON")
CMAKE_OPTIONS+=("-DCMAKE_CXX_EXTENSIONS=OFF")

CMAKE_BUILD_ARGS=(--parallel)

if [[ "${OTELCPP_CMAKE_VERBOSE_BUILD:-OFF}" =~ ^(1|ON|on|TRUE|true|YES|yes)$ ]]; then
  CMAKE_BUILD_ARGS+=(--verbose)
fi

if command -v ninja >/dev/null 2>&1; then
  CMAKE_OPTIONS+=("-G" "Ninja")
fi

if [ -n "$CMAKE_TOOLCHAIN_FILE" ]; then
  echo "CMAKE_TOOLCHAIN_FILE is set to: $CMAKE_TOOLCHAIN_FILE"
  CMAKE_OPTIONS+=("-DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE")
fi

if [ -n "$OTELCPP_CMAKE_CACHE_FILE" ]; then
  OTELCPP_CMAKE_CACHE_FILE_PATH="${SRC_DIR}/test_common/cmake/${OTELCPP_CMAKE_CACHE_FILE}"
else
  OTELCPP_CMAKE_CACHE_FILE_PATH="${SRC_DIR}/test_common/cmake/all-options-abiv1-preview.cmake"
fi

export CTEST_OUTPUT_ON_FAILURE=1

if [[ "$1" == "cmake.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DWITH_PROMETHEUS=ON \
        -DWITH_ZIPKIN=ON \
        -DWITH_ELASTICSEARCH=ON \
        -DWITH_METRICS_EXEMPLAR_PREVIEW=ON \
        -DCMAKE_CXX_FLAGS="-Werror $CXXFLAGS" \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.maintainer.sync.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -C ${SRC_DIR}/test_common/cmake/all-options-abiv1-preview.cmake \
        -DWITH_OPENTRACING=OFF \
        -DWITH_ASYNC_EXPORT_PREVIEW=OFF \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DWITH_NO_DEPRECATED_CODE=ON \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.maintainer.async.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -C ${SRC_DIR}/test_common/cmake/all-options-abiv1-preview.cmake \
        -DWITH_OPENTRACING=OFF \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DWITH_NO_DEPRECATED_CODE=ON \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.maintainer.abiv2.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -C ${SRC_DIR}/test_common/cmake/all-options-abiv2-preview.cmake \
        -DWITH_OPENTRACING=OFF \
        -DWITH_ASYNC_EXPORT_PREVIEW=OFF \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DWITH_NO_DEPRECATED_CODE=ON \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.maintainer.yaml.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DWITH_OTLP_HTTP=ON \
        -DWITH_OTLP_GRPC=ON \
        -DWITH_OTLP_FILE=ON \
        -DWITH_PROMETHEUS=ON \
        -DWITH_EXAMPLES=ON \
        -DWITH_EXAMPLES_HTTP=ON \
        -DWITH_ZIPKIN=ON \
        -DBUILD_W3CTRACECONTEXT_TEST=ON \
        -DWITH_ELASTICSEARCH=ON \
        -DWITH_METRICS_EXEMPLAR_PREVIEW=ON \
        -DWITH_ASYNC_EXPORT_PREVIEW=OFF \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DWITH_NO_DEPRECATED_CODE=ON \
        -DWITH_OTLP_HTTP_COMPRESSION=ON \
        -DWITH_OTLP_RETRY_PREVIEW=ON \
        -DWITH_THREAD_INSTRUMENTATION_PREVIEW=ON \
        -DWITH_CONFIGURATION=ON \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.with_async_export.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DWITH_PROMETHEUS=ON \
        -DWITH_ZIPKIN=ON \
        -DWITH_ELASTICSEARCH=ON \
        -DWITH_METRICS_EXEMPLAR_PREVIEW=ON \
        -DCMAKE_CXX_FLAGS="-Werror $CXXFLAGS" \
        -DWITH_ASYNC_EXPORT_PREVIEW=ON \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.opentracing_shim.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}" \
        -DCMAKE_CXX_FLAGS="-Werror -Wno-error=redundant-move $CXXFLAGS" \
        -DWITH_OPENTRACING=ON \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.opentracing_shim.install.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  rm -rf ${INSTALL_TEST_DIR}/*
  cmake "${CMAKE_OPTIONS[@]}" \
        -DCMAKE_CXX_FLAGS="-Werror -Wno-error=redundant-move $CXXFLAGS" \
        -DWITH_OPENTRACING=ON \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DCMAKE_INSTALL_PREFIX=${INSTALL_TEST_DIR} \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  cmake --build . "${CMAKE_BUILD_ARGS[@]}" --target install
  export LD_LIBRARY_PATH="${INSTALL_TEST_DIR}/lib:$LD_LIBRARY_PATH"
  CMAKE_OPTIONS_STRING=$(IFS=" "; echo "${CMAKE_OPTIONS[*]}")
  EXPECTED_COMPONENTS=(
    "api"
    "sdk"
    "ext_common"
    "exporters_in_memory"
    "exporters_ostream"
    "shims_opentracing"
  )
  EXPECTED_COMPONENTS_STRING=$(IFS=\;; echo "${EXPECTED_COMPONENTS[*]}")
  mkdir -p "${BUILD_DIR}/install_test"
  cd "${BUILD_DIR}/install_test"
  cmake "${CMAKE_OPTIONS[@]}" \
        "-DCMAKE_PREFIX_PATH=${INSTALL_TEST_DIR}" \
        "-DINSTALL_TEST_CMAKE_OPTIONS=${CMAKE_OPTIONS_STRING}" \
        "-DINSTALL_TEST_COMPONENTS=${EXPECTED_COMPONENTS_STRING}" \
        -S "${SRC_DIR}/install/test/cmake"
  ctest --output-on-failure
  exit 0
 elif [[ "$1" == "cmake.c++14.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DCMAKE_CXX_STANDARD=14 \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DWITH_ASYNC_EXPORT_PREVIEW=ON \
        -DWITH_STL=OFF \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.c++17.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DCMAKE_CXX_STANDARD=17 \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DWITH_ASYNC_EXPORT_PREVIEW=ON \
        -DWITH_STL=OFF \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.c++20.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DCMAKE_CXX_STANDARD=20 \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DWITH_ASYNC_EXPORT_PREVIEW=ON \
        -DWITH_STL=OFF \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.c++23.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DCMAKE_CXX_STANDARD=23 \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DWITH_ASYNC_EXPORT_PREVIEW=ON \
        -DWITH_STL=OFF \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.c++14.stl.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DWITH_METRICS_EXEMPLAR_PREVIEW=ON \
        -DCMAKE_CXX_STANDARD=14 \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DWITH_ASYNC_EXPORT_PREVIEW=ON \
        -DWITH_STL=CXX14 \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.c++17.stl.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DWITH_METRICS_EXEMPLAR_PREVIEW=ON \
        -DCMAKE_CXX_STANDARD=17 \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DWITH_ASYNC_EXPORT_PREVIEW=ON \
        -DWITH_STL=CXX17 \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.c++20.stl.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DWITH_METRICS_EXEMPLAR_PREVIEW=ON \
        -DCMAKE_CXX_STANDARD=20 \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DWITH_ASYNC_EXPORT_PREVIEW=ON \
        -DWITH_STL=CXX20 \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.c++23.stl.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DWITH_METRICS_EXEMPLAR_PREVIEW=ON \
        -DCMAKE_CXX_STANDARD=23 \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DWITH_ASYNC_EXPORT_PREVIEW=ON \
        -DWITH_STL=CXX23 \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.clang_tidy.test" ]]; then
  rm -rf "${BUILD_DIR}"
  mkdir -p "${BUILD_DIR}"
  clang-tidy --version
  LOG_FILE="${BUILD_DIR}/opentelemetry-cpp-clang-tidy.log"
  cmake "${CMAKE_OPTIONS[@]}"  \
    -S ${SRC_DIR} \
    -B ${BUILD_DIR} \
    -C ${OTELCPP_CMAKE_CACHE_FILE_PATH} \
    -DWITH_OPENTRACING=OFF \
    -DCMAKE_CXX_FLAGS="-Wno-deprecated-declarations" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_CXX_CLANG_TIDY="clang-tidy;--header-filter=.*/opentelemetry-cpp/.*;--exclude-header-filter=.*(internal/absl|third_party|third-party|build.*|/usr|/opt)/.*|.*\.pb\.h;--quiet"
  cmake --build "${BUILD_DIR}" "${CMAKE_BUILD_ARGS[@]}" 2>&1 | tee "$LOG_FILE"
  if [ ! -s "$LOG_FILE" ]; then
    echo "Error: Build log was not created at $LOG_FILE"
    exit 1
  fi
  echo "Build log written to: $LOG_FILE"
  echo "To generate a clang-tidy report, use the following command:"
  echo "  python3 ./ci/create_clang_tidy_report.py --build_log $LOG_FILE"
  exit 0
elif [[ "$1" == "cmake.exporter.otprotocol.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DWITH_OTLP_GRPC=ON \
        -DWITH_OTLP_HTTP=ON \
        -DWITH_OTLP_FILE=ON \
        -DWITH_OTLP_GRPC_SSL_MTLS_PREVIEW=ON \
        -DWITH_OTLP_GRPC_CREDENTIAL_PREVIEW=ON \
        -DWITH_OTLP_RETRY_PREVIEW=ON \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.exporter.otprotocol.shared_libs.with_static_grpc.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DWITH_OTLP_GRPC=ON \
        -DWITH_OTLP_HTTP=ON \
        -DWITH_OTLP_FILE=ON \
        -DBUILD_SHARED_LIBS=ON \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.exporter.otprotocol.with_async_export.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DWITH_OTLP_GRPC=ON \
        -DWITH_OTLP_HTTP=ON \
        -DWITH_OTLP_FILE=ON \
        -DWITH_ASYNC_EXPORT_PREVIEW=ON \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.w3c.trace-context.build-server" ]]; then
  echo "Building w3c trace context test server"
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
          -DBUILD_W3CTRACECONTEXT_TEST=ON \
          -DCMAKE_CXX_STANDARD=${CXX_STANDARD} \
          "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  exit 0
elif [[ "$1" == "cmake.do_not_install.test" ]]; then
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DWITH_OTLP_GRPC=ON \
        -DWITH_OTLP_HTTP=ON \
        -DWITH_OTLP_FILE=ON \
        -DWITH_ASYNC_EXPORT_PREVIEW=ON \
        -DOPENTELEMETRY_INSTALL=OFF \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.install.test" ]]; then
  if [[ -n "${BUILD_SHARED_LIBS}" && "${BUILD_SHARED_LIBS}" == "ON" ]]; then
    CMAKE_OPTIONS+=("-DBUILD_SHARED_LIBS=ON")
    echo "BUILD_SHARED_LIBS is set to: ON"
  else
    CMAKE_OPTIONS+=("-DBUILD_SHARED_LIBS=OFF")
    echo "BUILD_SHARED_LIBS is set to: OFF"
  fi
  CMAKE_OPTIONS+=("-DCMAKE_POSITION_INDEPENDENT_CODE=ON")

  cd "${BUILD_DIR}"
  rm -rf *
  rm -rf ${INSTALL_TEST_DIR}/*

  cmake "${CMAKE_OPTIONS[@]}"  \
        -DCMAKE_INSTALL_PREFIX=${INSTALL_TEST_DIR} \
        -C ${SRC_DIR}/test_common/cmake/all-options-abiv2-preview.cmake \
        -DOPENTELEMETRY_INSTALL=ON \
        "${SRC_DIR}"

  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  cmake --build . "${CMAKE_BUILD_ARGS[@]}" --target install
  export LD_LIBRARY_PATH="${INSTALL_TEST_DIR}/lib:$LD_LIBRARY_PATH"

  CMAKE_OPTIONS_STRING=$(IFS=" "; echo "${CMAKE_OPTIONS[*]}")

  EXPECTED_COMPONENTS=(
    "api"
    "sdk"
    "configuration"
    "ext_common"
    "ext_http_curl"
    "exporters_in_memory"
    "exporters_ostream"
    "exporters_ostream_builder"
    "exporters_otlp_common"
    "exporters_otlp_file"
    "exporters_otlp_file_builder"
    "exporters_otlp_grpc"
    "exporters_otlp_grpc_builder"
    "exporters_otlp_http"
    "exporters_otlp_http_builder"
    "exporters_prometheus"
    "exporters_prometheus_builder"
    "exporters_elasticsearch"
    "exporters_zipkin"
    "resource_detectors"
  )
  EXPECTED_COMPONENTS_STRING=$(IFS=\;; echo "${EXPECTED_COMPONENTS[*]}")
  mkdir -p "${BUILD_DIR}/install_test"
  cd "${BUILD_DIR}/install_test"
  cmake  "${CMAKE_OPTIONS[@]}" \
         "-DCMAKE_PREFIX_PATH=${INSTALL_TEST_DIR}" \
         "-DINSTALL_TEST_CMAKE_OPTIONS=${CMAKE_OPTIONS_STRING}" \
         "-DINSTALL_TEST_COMPONENTS=${EXPECTED_COMPONENTS_STRING}" \
         -S "${SRC_DIR}/install/test/cmake"
  ctest --output-on-failure
  exit 0
elif [[ "$1" == "cmake.fetch_content.test" ]]; then
  if [[ -n "${BUILD_SHARED_LIBS}" && "${BUILD_SHARED_LIBS}" == "ON" ]]; then
    CMAKE_OPTIONS+=("-DBUILD_SHARED_LIBS=ON")
    echo "BUILD_SHARED_LIBS is set to: ON"
  else
    CMAKE_OPTIONS+=("-DBUILD_SHARED_LIBS=OFF")
    echo "BUILD_SHARED_LIBS is set to: OFF"
  fi
  CMAKE_OPTIONS+=("-DCMAKE_POSITION_INDEPENDENT_CODE=ON")

  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DCMAKE_INSTALL_PREFIX=${INSTALL_TEST_DIR} \
        -C ${SRC_DIR}/test_common/cmake/all-options-abiv2-preview.cmake \
        -DOPENTELEMETRY_INSTALL=OFF \
        -DOPENTELEMETRY_CPP_SRC_DIR="${SRC_DIR}" \
        "${SRC_DIR}/install/test/cmake/fetch_content_test"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
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
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DOTELCPP_MAINTAINER_MODE=ON \
        -DCMAKE_EXE_LINKER_FLAGS="$LINKER_FLAGS" \
        -DCMAKE_SHARED_LINKER_FLAGS="$LINKER_FLAGS" \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}" --target example_plugin
  cp examples/plugin/plugin/libexample_plugin.so ${PLUGIN_DIR}

  # Verify we can load the plugin
  cd "${BUILD_DIR}"
  rm -rf *
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DOTELCPP_MAINTAINER_MODE=ON \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}" --target load_plugin_example
  examples/plugin/load/load_plugin_example ${PLUGIN_DIR}/libexample_plugin.so /dev/null
  exit 0
elif [[ "$1" == "bazel.test" ]]; then
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS $BAZEL_WITH_PREVIEW //...
  bazel $BAZEL_STARTUP_OPTIONS test $BAZEL_TEST_OPTIONS $BAZEL_WITH_PREVIEW //...
  exit 0
elif [[ "$1" == "bazel.with_async_export.test" ]]; then
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS_ASYNC //...
  bazel $BAZEL_STARTUP_OPTIONS test $BAZEL_TEST_OPTIONS_ASYNC //...
  exit 0
elif [[ "$1" == "bazel.benchmark" ]]; then
  run_benchmarks
  exit 0
elif [[ "$1" == "bazel.macos.test" ]]; then
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_MACOS_OPTIONS -- //...
  bazel $BAZEL_STARTUP_OPTIONS test $BAZEL_MACOS_TEST_OPTIONS -- //...
  exit 0
elif [[ "$1" == "bazel.legacy.test" ]]; then
  # we uses C++ future and async() function to test the Prometheus Exporter functionality,
  # that make this test always fail. ignore Prometheus exporter here.
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS_ASYNC -- //... -//exporters/otlp/... -//exporters/prometheus/...
  bazel $BAZEL_STARTUP_OPTIONS test $BAZEL_TEST_OPTIONS_ASYNC -- //... -//exporters/otlp/... -//exporters/prometheus/...
  exit 0
elif [[ "$1" == "bazel.noexcept" ]]; then
  # there are some exceptions and error handling code from the Prometheus Client
  # as well as Opentracing shim (due to some third party code in its Opentracing dependency)
  # that make this test always fail. Ignore these packages in the noexcept test here.
  # Set the api:with_cxx_stdlib=none because C++17 std::variant::get<> throws

  bazel $BAZEL_STARTUP_OPTIONS build --copt=-fno-exceptions --//api:with_cxx_stdlib=none $BAZEL_OPTIONS_ASYNC -- //... -//exporters/prometheus/... -//examples/prometheus/... -//opentracing-shim/... -//examples/configuration/... -//sdk/src/configuration/... -//sdk/test/configuration/...
  bazel $BAZEL_STARTUP_OPTIONS test --copt=-fno-exceptions --//api:with_cxx_stdlib=none $BAZEL_TEST_OPTIONS_ASYNC -- //... -//exporters/prometheus/... -//examples/prometheus/... -//opentracing-shim/... -//examples/configuration/... -//sdk/src/configuration/... -//sdk/test/configuration/...
  exit 0
elif [[ "$1" == "bazel.nortti" ]]; then
  # there are some exceptions and error handling code from the Prometheus Client
  # that make this test always fail. Ignore these packages in the nortti test here.
  bazel $BAZEL_STARTUP_OPTIONS build --cxxopt=-fno-rtti $BAZEL_OPTIONS_ASYNC -- //... -//exporters/prometheus/...
  bazel $BAZEL_STARTUP_OPTIONS test --cxxopt=-fno-rtti $BAZEL_TEST_OPTIONS_ASYNC -- //... -//exporters/prometheus/...
  exit 0
elif [[ "$1" == "bazel.asan" ]]; then
  bazel $BAZEL_STARTUP_OPTIONS test --config=asan $BAZEL_TEST_OPTIONS_ASYNC //...
  exit 0
elif [[ "$1" == "bazel.ubsan" ]]; then
  bazel $BAZEL_STARTUP_OPTIONS test --config=ubsan $BAZEL_TEST_OPTIONS_ASYNC //...
  exit 0
elif [[ "$1" == "bazel.tsan" ]]; then
# TODO - potential race condition in Civetweb server used by prometheus-cpp during shutdown
# https://github.com/civetweb/civetweb/issues/861, so removing prometheus from the test
  bazel $BAZEL_STARTUP_OPTIONS test --config=tsan $BAZEL_TEST_OPTIONS_ASYNC  -- //... -//exporters/prometheus/...
  exit 0
elif [[ "$1" == "bazel.valgrind" ]]; then
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS_ASYNC //...
  bazel $BAZEL_STARTUP_OPTIONS test --test_timeout=600 --run_under="/usr/bin/valgrind --leak-check=full --error-exitcode=1 --errors-for-leak-kinds=definite --suppressions=\"${SRC_DIR}/ci/valgrind-suppressions\"" $BAZEL_TEST_OPTIONS_ASYNC //...
  exit 0
elif [[ "$1" == "benchmark" ]]; then
  [ -z "${BENCHMARK_DIR}" ] && export BENCHMARK_DIR=$HOME/benchmark
  bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS_ASYNC -c opt -- \
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
  cmake "${CMAKE_OPTIONS[@]}"  \
        -DCMAKE_CXX_FLAGS="-Werror --coverage $CXXFLAGS" \
        "${SRC_DIR}"
  cmake --build . "${CMAKE_BUILD_ARGS[@]}"
  ctest --output-on-failure
  lcov --directory $PWD --capture --output-file coverage.info
  # removing test http server coverage from the total coverage. We don't use this server completely.
  lcov --remove coverage.info '*/ext/http/server/*'> tmp_coverage.info 2>/dev/null
  cp tmp_coverage.info coverage.info
  exit 0
fi

echo "Invalid do_ci.sh target, see ci/README.md for valid targets."
exit 1
