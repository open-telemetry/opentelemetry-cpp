# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)
project(opentelemetry-cpp-exporters-zipkin-install-test LANGUAGES CXX)

find_package(opentelemetry-cpp REQUIRED COMPONENTS exporters_zipkin)

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(exporters_zipkin_test
               ${INSTALL_TEST_SRC_DIR}/test_exporters_zipkin.cc)
target_link_libraries(
  exporters_zipkin_test PRIVATE opentelemetry-cpp::zipkin_trace_exporter
                                GTest::GTest GTest::Main)

gtest_discover_tests(exporters_zipkin_test)
