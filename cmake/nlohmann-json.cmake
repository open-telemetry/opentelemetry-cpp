# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

otel_add_thirdparty_package(
  PACKAGE_NAME "nlohmann_json"
  PACKAGE_SEARCH_MODES "CONFIG"
  FETCH_GIT_REPOSITORY "https://github.com/nlohmann/json.git"
  FETCH_GIT_TAG "${nlohmann-json_GIT_TAG}"
  FETCH_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/nlohmann-json"
  FETCH_CMAKE_ARGS
    "-DJSON_BuildTests=OFF"
    "-DJSON_Install=${OPENTELEMETRY_INSTALL}"
    "-DJSON_MultipleHeaders=OFF"
  REQUIRED_TARGETS "nlohmann_json::nlohmann_json"
  VERSION_REGEX "project\\([^\\)]*VERSION[ \t]*([0-9]+(\\.[0-9]+)*(\\.[0-9]+)*)"
  VERSION_FILE "\${nlohmann_json_SOURCE_DIR}/CMakeLists.txt"
)
