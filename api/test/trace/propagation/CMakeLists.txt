# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(detail)

foreach(testname http_text_format_test b3_propagation_test
                 jaeger_propagation_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(${testname} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
  gtest_discover_tests(${testname} TEST_PREFIX trace.)
endforeach()

if(OTELCPP_WITH_BENCHMARK)
  add_executable(propagator_benchmark propagator_benchmark.cc)
  target_link_libraries(propagator_benchmark benchmark::benchmark
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
endif()
