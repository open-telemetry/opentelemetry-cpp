add_library(opentelemetry_exporter_stdout stdout.cc)
target_link_libraries(opentelemetry_exporter_stdout)

add_executable(stdout_span_test stdout_span_test.cc)
target_link_libraries(stdout_span_test
                      ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      opentelemetry_exporter_stdout)
gtest_add_tests(TARGET stdout_span_test TEST_PREFIX exporter. TEST_LIST stdout_span_test)