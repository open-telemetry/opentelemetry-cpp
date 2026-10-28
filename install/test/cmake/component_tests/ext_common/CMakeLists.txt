# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)
project(opentelemetry-cpp-ext_common-install-test LANGUAGES CXX)

find_package(opentelemetry-cpp REQUIRED COMPONENTS ext_common)

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

# One source per header the ext_common component installs, listed rather than
# globbed so that adding or removing an installed header has to be done here
# too.
add_executable(
  ext_common_test
  ${INSTALL_TEST_SRC_DIR}/test_ext_common.cc
  ${INSTALL_TEST_SRC_DIR}/installed_headers/ext_common/http_client.cc
  ${INSTALL_TEST_SRC_DIR}/installed_headers/ext_common/http_client_factory.cc
  ${INSTALL_TEST_SRC_DIR}/installed_headers/ext_common/http_client_factory_curl.cc
  ${INSTALL_TEST_SRC_DIR}/installed_headers/ext_common/url_parser.cc)
target_link_libraries(ext_common_test PRIVATE opentelemetry-cpp::ext
                                              GTest::gtest GTest::gtest_main)

gtest_discover_tests(ext_common_test)
