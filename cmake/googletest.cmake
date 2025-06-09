# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

otel_add_thirdparty_package(
  PACKAGE_NAME "GTest"
  PACKAGE_SEARCH_MODES "CONFIG"
  FETCH_NAME googletest
  FETCH_GIT_REPOSITORY "https://github.com/google/googletest.git"
  FETCH_GIT_TAG "${googletest_GIT_TAG}"
  FETCH_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/googletest"
  FETCH_CMAKE_ARGS
    "-DINSTALL_GTEST=${OPENTELEMETRY_INSTALL}"
    "-DBUILD_GMOCK=ON"
  REQUIRED_TARGETS "GTest::gtest;GTest::gtest_main;GTest::gmock"
  VERSION_REGEX "set\\s*\\(\\s*GOOGLETEST_VERSION[ \t]+([0-9]+(\\.[0-9]+)*)([ \t]|\\))"
  VERSION_FILE "\${googletest_SOURCE_DIR}/CMakeLists.txt"
)

if(NOT GTEST_BOTH_LIBRARIES)
  set(GTEST_BOTH_LIBRARIES GTest::gtest GTest::gtest_main)
endif()

if(NOT GMOCK_LIB)
  set(GMOCK_LIB GTest::gmock)
endif()
