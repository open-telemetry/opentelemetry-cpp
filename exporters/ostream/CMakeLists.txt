include_directories(include)

add_library(opentelemetry_exporter_ostream_metrics src/metrics_exporter.cc)

add_executable(ostream_metrics_test test/ostream_metrics_test.cc)
target_link_libraries(ostream_metrics_test
                      ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      opentelemetry_exporter_ostream_metrics)
gtest_add_tests(TARGET ostream_metrics_test TEST_PREFIX exporter. TEST_LIST ostream_metrics_test)