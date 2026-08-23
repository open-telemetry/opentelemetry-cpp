# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)
project(opentelemetry-cpp-ext_http-install-test LANGUAGES CXX)

find_package(opentelemetry-cpp REQUIRED COMPONENTS ext_http)

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(ext_http_test ${INSTALL_TEST_SRC_DIR}/test_ext_http.cc)
target_link_libraries(ext_http_test PRIVATE opentelemetry-cpp::http_client
                                            GTest::gtest GTest::gtest_main)

gtest_discover_tests(ext_http_test)
