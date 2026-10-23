# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_executable(registry_test registry_test.cc)
target_link_libraries(
  registry_test PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
                        opentelemetry_configuration_core)
gtest_discover_tests(registry_test TEST_PREFIX config.)

add_executable(sdk_builder_test sdk_builder_test.cc)
target_link_libraries(
  sdk_builder_test PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
                           opentelemetry_configuration_core)
gtest_discover_tests(sdk_builder_test TEST_PREFIX config.)

add_executable(trace_builders_test trace_builders_test.cc)
target_link_libraries(
  trace_builders_test
  PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
          opentelemetry_configuration_trace_builders
          opentelemetry_configuration_core)
gtest_discover_tests(trace_builders_test TEST_PREFIX config.)

add_executable(logs_builders_test logs_builders_test.cc)
target_link_libraries(
  logs_builders_test
  PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
          opentelemetry_configuration_logs_builders
          opentelemetry_configuration_core)
gtest_discover_tests(logs_builders_test TEST_PREFIX config.)

add_executable(metrics_builders_test metrics_builders_test.cc)
target_link_libraries(
  metrics_builders_test
  PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
          opentelemetry_configuration_metrics_builders
          opentelemetry_configuration_core opentelemetry_test_common)
gtest_discover_tests(metrics_builders_test TEST_PREFIX config.)

add_executable(propagator_builders_test propagator_builders_test.cc)
target_link_libraries(
  propagator_builders_test
  PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
          opentelemetry_configuration_core)
gtest_discover_tests(propagator_builders_test TEST_PREFIX config.)

foreach(testname configured_sdk_test programmatic_configuration_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(
    ${testname} PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
                        opentelemetry_configuration_registry_factory)
  gtest_discover_tests(${testname} TEST_PREFIX config.)
endforeach()

if(WITH_CONFIGURATION)
  foreach(
    testname
    yaml_logs_test
    yaml_metrics_test
    yaml_propagator_test
    yaml_resource_test
    yaml_test
    yaml_trace_test
    yaml_distribution_test)
    add_executable(${testname} "${testname}.cc")
    target_link_libraries(
      ${testname} PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
                          opentelemetry_configuration_yaml)
    gtest_discover_tests(${testname} TEST_PREFIX yaml.)
  endforeach()
endif()
