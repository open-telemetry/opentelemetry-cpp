foreach(
  testname
  meter_provider_sdk_test
  gauge_aggregator_test
  min_max_sum_count_aggregator_test
  exact_aggregator_test
  counter_aggregator_test
  histogram_aggregator_test
  ungrouped_processor_test
  meter_test
  metric_instrument_test
  controller_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(
    ${testname} ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_metrics_deprecated)
  gtest_add_tests(
    TARGET ${testname}
    TEST_PREFIX metrics.
    TEST_LIST ${testname})
endforeach()
