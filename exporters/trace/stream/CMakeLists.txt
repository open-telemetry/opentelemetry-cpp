add_library(opentelemetry_exporter_stream_span stream.cc)
target_link_libraries(opentelemetry_exporter_stream_span)

add_executable(stream_span_test stream_span_test.cc)
target_link_libraries(stream_span_test
                      ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      opentelemetry_exporter_stream_span)
gtest_add_tests(TARGET stream_span_test TEST_PREFIX exporter. TEST_LIST stream_span_test)