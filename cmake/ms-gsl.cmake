# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

otel_add_thirdparty_package(
  PACKAGE_NAME "Microsoft.GSL"
  PACKAGE_SEARCH_MODES "CONFIG"
  FETCH_NAME "gsl"
  FETCH_GIT_REPOSITORY "https://github.com/microsoft/GSL.git"
  FETCH_GIT_TAG "${ms-gsl_GIT_TAG}"
  FETCH_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/ms-gsl"
  FETCH_CMAKE_ARGS
    "-DGSL_TEST=OFF"
    "-DGSL_INSTALL=${OPENTELEMETRY_INSTALL}"
  REQUIRED_TARGETS "Microsoft.GSL::GSL"
  VERSION_REGEX "project\\([^\\)]*VERSION[ \t]*([0-9]+(\\.[0-9]+)*(\\.[0-9]+)*)"
  VERSION_FILE "\${GSL_SOURCE_DIR}/CMakeLists.txt"
)
