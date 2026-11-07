# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(WITH_BENCHMARK)
  add_executable(fixed_size_exemplar_reservoir_benchmark
                 fixed_size_exemplar_reservoir_benchmark.cc)
  target_link_libraries(
    fixed_size_exemplar_reservoir_benchmark benchmark::benchmark
    ${CMAKE_THREAD_LIBS_INIT} opentelemetry_common)
endif()

foreach(
  testname
  no_exemplar_reservoir_test aligned_histogram_bucket_exemplar_reservoir_test
  reservoir_cell_test filter_predicate_test fixed_size_exemplar_reservoir_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(
    ${testname} ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_metrics opentelemetry_metrics)
  gtest_add_tests(
    TARGET ${testname}
    TEST_PREFIX metrics.
    TEST_LIST ${testname})
endforeach()
