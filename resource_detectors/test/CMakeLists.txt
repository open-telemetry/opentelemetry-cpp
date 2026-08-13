# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_executable(
  resource_detector_test container_detector_test.cc host_detector_test.cc
                         process_detector_test.cc env_entity_detector_test.cc)

# Link the required dependencies
target_link_libraries(
  resource_detector_test
  PRIVATE opentelemetry_resource_detectors opentelemetry_api
          opentelemetry_resources GTest::gtest_main)

gtest_add_tests(
  TARGET resource_detector_test
  TEST_PREFIX resource_detector.
  TEST_LIST resource_detector_test)

add_executable(resource_detector_builder_test container_detector_builder_test.cc
                                              process_detector_builder_test.cc)

target_link_libraries(
  resource_detector_builder_test
  PRIVATE opentelemetry_resource_detectors
          opentelemetry_resource_detectors_builders
          opentelemetry_api
          opentelemetry_resources
          opentelemetry_configuration_core
          GTest::gtest_main)

gtest_add_tests(
  TARGET resource_detector_builder_test
  TEST_PREFIX resource_detector.
  TEST_LIST resource_detector_builder_test)
