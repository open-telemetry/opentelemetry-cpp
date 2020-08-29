include_directories(include)

add_library(opentelemetry_exporter_ostream_metrics src/metrics_exporter.cc)
add_library(opentelemetry_exporter_ostream_span src/span_exporter.cc)

if(BUILD_TESTING)
  add_executable(ostream_metrics_test test/ostream_metrics_test.cc)
  add_executable(ostream_span_test test/ostream_span_test.cc)

  target_link_libraries(
    ostream_span_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_exporter_ostream_span)

  target_link_libraries(
    ostream_metrics_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_exporter_ostream_metrics)

  gtest_add_tests(TARGET ostream_metrics_test TEST_PREFIX exporter. TEST_LIST
                  ostream_metrics_test)
  gtest_add_tests(TARGET ostream_span_test TEST_PREFIX exporter. TEST_LIST
                  ostream_span_test)
endif() # BUILD_TESTING
