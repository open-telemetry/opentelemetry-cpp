# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

otel_add_thirdparty_package(
  PACKAGE_NAME "prometheus-cpp"
  PACKAGE_SEARCH_MODES "CONFIG"
  FETCH_GIT_REPOSITORY "https://github.com/jupp0r/prometheus-cpp.git"
  FETCH_GIT_TAG "${prometheus-cpp_GIT_TAG}"
  FETCH_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/prometheus-cpp"
  FETCH_CMAKE_ARGS
    ENABLE_TESTING=OFF
    ENABLE_PUSH=OFF
  REQUIRED_TARGETS "prometheus-cpp::core;prometheus-cpp::pull"
  VERSION_REGEX "project\\([^\\)]*VERSION[ \t]*([0-9]+(\\.[0-9]+)*(\\.[0-9]+)*)"
  VERSION_FILE "\${prometheus-cpp_SOURCE_DIR}/CMakeLists.txt"
)

if(TARGET core)
  set_target_properties(core PROPERTIES
        CXX_INCLUDE_WHAT_YOU_USE ""
        CXX_CLANG_TIDY "")
endif()

if(TARGET pull)
  set_target_properties(pull PROPERTIES
        CXX_INCLUDE_WHAT_YOU_USE ""
        CXX_CLANG_TIDY "")
endif()
