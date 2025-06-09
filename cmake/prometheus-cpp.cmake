# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

otel_add_thirdparty_package(
  PACKAGE_NAME "prometheus-cpp"
  PACKAGE_SEARCH_MODES "CONFIG"
  FETCH_GIT_REPOSITORY "https://github.com/jupp0r/prometheus-cpp.git"
  FETCH_GIT_TAG "${prometheus-cpp_GIT_TAG}"
  FETCH_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/prometheus-cpp"
  FETCH_GIT_SUBMODULES "3rdparty/civetweb"
  FETCH_CMAKE_ARGS
    "-DENABLE_TESTING=OFF"
    "-DENABLE_PUSH=OFF"
    "-DUSE_THIRDPARTY_LIBRARIES=ON"
  REQUIRED_TARGETS "prometheus-cpp::core;prometheus-cpp::pull"
  VERSION_REGEX "project\\([^\\)]*VERSION[ \t]*([0-9]+(\\.[0-9]+)*(\\.[0-9]+)*)"
  VERSION_FILE "\${prometheus-cpp_SOURCE_DIR}/CMakeLists.txt"
)
