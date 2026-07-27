# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)
project(opentelemetry-cpp-ext_common-install-test LANGUAGES CXX)

find_package(opentelemetry-cpp REQUIRED COMPONENTS ext_common)

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(ext_common_test ${INSTALL_TEST_SRC_DIR}/test_ext_common.cc)
target_link_libraries(ext_common_test PRIVATE opentelemetry-cpp::ext
                                              GTest::gtest GTest::gtest_main)

gtest_discover_tests(ext_common_test)

include(${CMAKE_CURRENT_LIST_DIR}/../../otel_header_probes.cmake)

otel_add_header_probes(
  TARGET
  ext_common_header_probes
  LINK
  opentelemetry-cpp::ext
  HEADERS
  opentelemetry/ext/http/common/url_parser.h
  opentelemetry/ext/http/server/http_server.h
  opentelemetry/ext/http/server/socket_tools.h)
