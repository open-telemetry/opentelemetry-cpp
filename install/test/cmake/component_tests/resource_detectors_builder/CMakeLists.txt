# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)
project(opentelemetry-cpp-resource-detectors-builder-install-test LANGUAGES CXX)

find_package(opentelemetry-cpp REQUIRED COMPONENTS resource_detectors_builder)

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(resource_detectors_builder_test
               ${INSTALL_TEST_SRC_DIR}/test_resource_detectors_builder.cc)
target_link_libraries(
  resource_detectors_builder_test
  PRIVATE opentelemetry-cpp::resource_detectors_builder GTest::gtest
          GTest::gtest_main)

gtest_discover_tests(resource_detectors_builder_test)
