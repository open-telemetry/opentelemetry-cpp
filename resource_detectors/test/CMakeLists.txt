# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

#
# container_resource_detector tests
#

add_executable(container_resource_detector_test container_detector_test.cc)
target_link_libraries(
  container_resource_detector_test
  PRIVATE opentelemetry_container_resource_detector GTest::gtest_main)
gtest_discover_tests(container_resource_detector_test TEST_PREFIX resource_detector.)

add_executable(container_resource_detector_builder_test
               container_detector_builder_test.cc)
target_link_libraries(
  container_resource_detector_builder_test
  PRIVATE opentelemetry_container_resource_detector_builder
          opentelemetry_configuration_core GTest::gtest_main)
gtest_discover_tests(container_resource_detector_builder_test TEST_PREFIX resource_detector.)

#
# env_entity_resource_detector tests
#

add_executable(env_entity_resource_detector_test env_entity_detector_test.cc)
target_link_libraries(
  env_entity_resource_detector_test
  PRIVATE opentelemetry_env_entity_resource_detector GTest::gtest_main)
gtest_discover_tests(env_entity_resource_detector_test TEST_PREFIX resource_detector.)

#
# host_resource_detector tests
#

add_executable(host_resource_detector_test host_detector_test.cc)
target_link_libraries(
  host_resource_detector_test PRIVATE opentelemetry_host_resource_detector
                                      GTest::gtest_main)
gtest_discover_tests(host_resource_detector_test TEST_PREFIX resource_detector.)

add_executable(host_resource_detector_builder_test
               host_detector_builder_test.cc)
target_link_libraries(
  host_resource_detector_builder_test
  PRIVATE opentelemetry_host_resource_detector_builder
          opentelemetry_configuration_core GTest::gtest_main)
gtest_discover_tests(host_resource_detector_builder_test TEST_PREFIX resource_detector.)

#
# process_resource_detector tests
#

add_executable(process_resource_detector_test process_detector_test.cc)
target_link_libraries(
  process_resource_detector_test PRIVATE opentelemetry_process_resource_detector
                                         GTest::gtest_main)
gtest_discover_tests(process_resource_detector_test TEST_PREFIX resource_detector.)

add_executable(process_resource_detector_builder_test
               process_detector_builder_test.cc)
target_link_libraries(
  process_resource_detector_builder_test
  PRIVATE opentelemetry_process_resource_detector_builder
          opentelemetry_configuration_core GTest::gtest_main)
gtest_discover_tests(process_resource_detector_builder_test TEST_PREFIX resource_detector.)

#
# service_resource_detector_utils tests
#

add_executable(service_resource_detector_utils_test
               service_detector_utils_test.cc)
target_link_libraries(
  service_resource_detector_utils_test
  PRIVATE opentelemetry_service_resource_detector_utils
          opentelemetry_process_resource_detector GTest::gtest_main)
gtest_discover_tests(service_resource_detector_utils_test TEST_PREFIX resource_detector.)

#
# service_resource_detector tests
#

add_executable(service_resource_detector_test service_detector_test.cc)
target_link_libraries(
  service_resource_detector_test PRIVATE opentelemetry_service_resource_detector
                                         GTest::gtest_main)
gtest_discover_tests(service_resource_detector_test TEST_PREFIX resource_detector.)

#
# service_resource_detector_builder tests
#

add_executable(service_resource_detector_builder_test
               service_detector_builder_test.cc)
target_link_libraries(
  service_resource_detector_builder_test
  PRIVATE opentelemetry_service_resource_detector_builder
          opentelemetry_configuration_core GTest::gtest_main)
gtest_discover_tests(service_resource_detector_builder_test TEST_PREFIX resource_detector.)
