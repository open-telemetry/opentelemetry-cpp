# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

otel_add_thirdparty_package(
  PACKAGE_NAME "benchmark"
  PACKAGE_SEARCH_MODES "CONFIG"
  FETCH_GIT_REPOSITORY "https://github.com/google/benchmark.git"
  FETCH_GIT_TAG ${benchmark_GIT_TAG}
  FETCH_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/benchmark"
  FETCH_CMAKE_ARGS
    "-DBENCHMARK_ENABLE_TESTING=OFF"
    "-DBENCHMARK_ENABLE_INSTALL=${OPENTELEMETRY_INSTALL}"
  REQUIRED_TARGETS "benchmark::benchmark"
  VERSION_REGEX "project.*\\([^\\)]*VERSION[ \t]*([0-9]+(\\.[0-9]+)*(\\.[0-9]+)*)"
  VERSION_FILE "\${benchmark_SOURCE_DIR}/CMakeLists.txt"
)
