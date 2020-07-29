include_directories(include)

add_library(opentelemetry_exporter_ostream_span src/span_exporter.cc)

add_executable(ostream_span_test test/ostream_span_test.cc)
target_link_libraries(
  ostream_span_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
  opentelemetry_exporter_ostream_span)
gtest_add_tests(TARGET ostream_span_test TEST_PREFIX exporter. TEST_LIST
                ostream_span_test)
