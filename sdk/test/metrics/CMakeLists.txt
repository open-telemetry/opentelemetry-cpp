foreach(
  testname
  meter_provider_sdk_test
  view_registry_test
  aggregation_test
  attributes_processor_test
  attributes_hashmap_test
  sync_metric_storage_test
  async_metric_storage_test
  multi_metric_storage_test
  observer_result_test
  sync_instruments_test
  async_instruments_test
  metric_reader_test
  periodic_exporting_metric_reader_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(
    ${testname} ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_resources opentelemetry_metrics)
  gtest_add_tests(
    TARGET ${testname}
    TEST_PREFIX metrics.
    TEST_LIST ${testname})
endforeach()

add_executable(attributes_processor_benchmark attributes_processor_benchmark.cc)
target_link_libraries(attributes_processor_benchmark benchmark::benchmark
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_common)

add_executable(attributes_hashmap_benchmark attributes_hashmap_benchmark.cc)
target_link_libraries(attributes_hashmap_benchmark benchmark::benchmark
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_common)

add_subdirectory(exemplar)
