# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)
project(opentelemetry-cpp-resource-detectors-install-test LANGUAGES CXX)

find_package(opentelemetry-cpp REQUIRED COMPONENTS resource_detectors)

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(resource_detectors_test
               ${INSTALL_TEST_SRC_DIR}/test_resource_detectors.cc)
target_link_libraries(
  resource_detectors_test
  PRIVATE opentelemetry-cpp::resource_detectors
          opentelemetry-cpp::resource_detectors_builders
          opentelemetry-cpp::container_resource_detector
          opentelemetry-cpp::container_resource_detector_builder
          opentelemetry-cpp::env_entity_resource_detector
          opentelemetry-cpp::host_resource_detector
          opentelemetry-cpp::host_resource_detector_builder
          opentelemetry-cpp::process_resource_detector
          opentelemetry-cpp::process_resource_detector_builder
          opentelemetry-cpp::service_resource_detector
          opentelemetry-cpp::service_resource_detector_builder
          GTest::gtest
          GTest::gtest_main)

gtest_discover_tests(resource_detectors_test)
