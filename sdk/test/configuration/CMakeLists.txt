# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_executable(programmatic_configuration_test
               programmatic_configuration_test.cc)
target_link_libraries(
  programmatic_configuration_test
  PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
          opentelemetry_configuration_core opentelemetry_trace
          opentelemetry_logs opentelemetry_metrics)
gtest_add_tests(
  TARGET programmatic_configuration_test
  TEST_PREFIX configuration_core.
  TEST_LIST programmatic_configuration_test)

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
    target_link_libraries(${testname} ${GTEST_BOTH_LIBRARIES}
                          ${CMAKE_THREAD_LIBS_INIT} opentelemetry_configuration)
    gtest_add_tests(
      TARGET ${testname}
      TEST_PREFIX trace.
      TEST_LIST ${testname})
  endforeach()
endif()
