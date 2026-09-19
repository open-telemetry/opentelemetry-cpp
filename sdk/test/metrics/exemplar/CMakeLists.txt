# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

foreach(
  testname
  no_exemplar_reservoir_test aligned_histogram_bucket_exemplar_reservoir_test
  reservoir_attribute_offer_test reservoir_cell_test filter_predicate_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(
    ${testname} ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_metrics opentelemetry_metrics)
  gtest_add_tests(
    TARGET ${testname}
    TEST_PREFIX metrics.
    TEST_LIST ${testname})
endforeach()

if(OTELCPP_WITH_BENCHMARK)
  add_executable(exemplar_offer_benchmark exemplar_offer_benchmark.cc)
  target_link_libraries(exemplar_offer_benchmark benchmark::benchmark
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_metrics)
endif()
