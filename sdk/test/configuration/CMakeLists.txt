# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_executable(registry_test registry_test.cc)
target_link_libraries(
  registry_test PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
                        opentelemetry_configuration_core)
gtest_add_tests(
  TARGET registry_test
  TEST_PREFIX config.
  TEST_LIST registry_test)

add_executable(trace_builders_test trace_builders_test.cc)
target_link_libraries(
  trace_builders_test
  PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
          opentelemetry_configuration_trace_builders
          opentelemetry_configuration_core)
gtest_add_tests(
  TARGET trace_builders_test
  TEST_PREFIX config.
  TEST_LIST trace_builders_test)

add_executable(logs_builders_test logs_builders_test.cc)
target_link_libraries(
  logs_builders_test
  PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
          opentelemetry_configuration_logs_builders
          opentelemetry_configuration_core)
gtest_add_tests(
  TARGET logs_builders_test
  TEST_PREFIX config.
  TEST_LIST logs_builders_test)

add_executable(metrics_builders_test metrics_builders_test.cc)
target_link_libraries(
  metrics_builders_test
  PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
          opentelemetry_configuration_metrics_builders
          opentelemetry_configuration_core)
gtest_add_tests(
  TARGET metrics_builders_test
  TEST_PREFIX config.
  TEST_LIST metrics_builders_test)

foreach(testname sdk_builder_test configured_sdk_test
                 programmatic_configuration_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(
    ${testname} PRIVATE ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
                        opentelemetry_configuration_registry_factory)
  gtest_add_tests(
    TARGET ${testname}
    TEST_PREFIX config.
    TEST_LIST ${testname})
endforeach()

target_link_libraries(sdk_builder_test PRIVATE opentelemetry_test_common)

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
                          opentelemetry_configuration)
    gtest_add_tests(
      TARGET ${testname}
      TEST_PREFIX yaml.
      TEST_LIST ${testname})
  endforeach()
endif()
