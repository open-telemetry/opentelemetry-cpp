# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)
project(opentelemetry-cpp-exporters-ostream-builder-install-test LANGUAGES CXX)

find_package(opentelemetry-cpp REQUIRED COMPONENTS exporters_ostream_builder)

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(exporters_ostream_builder_test
               ${INSTALL_TEST_SRC_DIR}/test_exporters_ostream_builder.cc)
target_link_libraries(
  exporters_ostream_builder_test
  PRIVATE opentelemetry-cpp::ostream_log_record_exporter_builder
          opentelemetry-cpp::ostream_metrics_exporter_builder
          opentelemetry-cpp::ostream_span_exporter_builder
          GTest::gtest
          GTest::gtest_main)

gtest_discover_tests(exporters_ostream_builder_test)
