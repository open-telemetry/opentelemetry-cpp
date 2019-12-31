include(GoogleTest)

add_executable(string_view_test string_view_test.cc)
target_link_libraries(string_view_test ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
gtest_add_tests(TARGET string_view_test TEST_PREFIX nostd. TEST_LIST
                string_view_test)

add_executable(span_test span_test.cc)
target_link_libraries(span_test ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
gtest_add_tests(TARGET span_test TEST_PREFIX nostd. TEST_LIST
                span_test)
