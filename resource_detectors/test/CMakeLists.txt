# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

#
# container_resource_detector tests
#

add_executable(container_resource_detector_test container_detector_test.cc)
target_link_libraries(
  container_resource_detector_test
  PRIVATE opentelemetry_container_resource_detector GTest::gtest_main)
gtest_add_tests(
  TARGET container_resource_detector_test
  TEST_PREFIX resource_detector.
  TEST_LIST container_resource_detector_test)

add_executable(container_resource_detector_builder_test
               container_detector_builder_test.cc)
target_link_libraries(
  container_resource_detector_builder_test
  PRIVATE opentelemetry_container_resource_detector_builder
          opentelemetry_configuration_core GTest::gtest_main)
gtest_add_tests(
  TARGET container_resource_detector_builder_test
  TEST_PREFIX resource_detector.
  TEST_LIST container_resource_detector_builder_test)

#
# env_entity_resource_detector tests
#

add_executable(env_entity_resource_detector_test env_entity_detector_test.cc)
target_link_libraries(
  env_entity_resource_detector_test
  PRIVATE opentelemetry_env_entity_resource_detector GTest::gtest_main)
gtest_add_tests(
  TARGET env_entity_resource_detector_test
  TEST_PREFIX resource_detector.
  TEST_LIST env_entity_resource_detector_test)

#
# host_resource_detector tests
#

add_executable(host_resource_detector_test host_detector_test.cc)
target_link_libraries(
  host_resource_detector_test PRIVATE opentelemetry_host_resource_detector
                                      GTest::gtest_main)
gtest_add_tests(
  TARGET host_resource_detector_test
  TEST_PREFIX resource_detector.
  TEST_LIST host_resource_detector_test)

#
# process_resource_detector tests
#

add_executable(process_resource_detector_test process_detector_test.cc)
target_link_libraries(
  process_resource_detector_test PRIVATE opentelemetry_process_resource_detector
                                         GTest::gtest_main)
gtest_add_tests(
  TARGET process_resource_detector_test
  TEST_PREFIX resource_detector.
  TEST_LIST process_resource_detector_test)

add_executable(process_resource_detector_builder_test
               process_detector_builder_test.cc)
target_link_libraries(
  process_resource_detector_builder_test
  PRIVATE opentelemetry_process_resource_detector_builder
          opentelemetry_configuration_core GTest::gtest_main)
gtest_add_tests(
  TARGET process_resource_detector_builder_test
  TEST_PREFIX resource_detector.
  TEST_LIST process_resource_detector_builder_test)
