# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)
project(opentelemetry-cpp-exporters-otlp-http-builder-install-test
        LANGUAGES CXX)

find_package(opentelemetry-cpp REQUIRED COMPONENTS exporters_otlp_http_builder)

if(NOT TARGET protobuf::libprotobuf)
  message(FATAL_ERROR "protobuf::libprotobuf target not found")
endif()

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(exporters_otlp_http_builder_test
               ${INSTALL_TEST_SRC_DIR}/test_exporters_otlp_http_builder.cc)
target_link_libraries(
  exporters_otlp_http_builder_test
  PRIVATE opentelemetry-cpp::otlp_http_exporter_builder
          opentelemetry-cpp::otlp_http_log_record_exporter_builder
          opentelemetry-cpp::otlp_http_metric_exporter_builder
          GTest::gtest
          GTest::gtest_main)

gtest_discover_tests(exporters_otlp_http_builder_test)
