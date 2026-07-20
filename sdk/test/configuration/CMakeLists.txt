# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

foreach(testname sdk_builder_test configured_sdk_test
                 programmatic_configuration_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(
    ${testname} PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
                        opentelemetry_configuration)
  gtest_add_tests(
    TARGET ${testname}
    TEST_PREFIX config.
    TEST_LIST ${testname})
endforeach()

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
                        opentelemetry_configuration)
  gtest_add_tests(
    TARGET ${testname}
    TEST_PREFIX yaml.
    TEST_LIST ${testname})
endforeach()
