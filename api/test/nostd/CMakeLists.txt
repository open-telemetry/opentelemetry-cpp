include(GoogleTest)

add_executable(string_view_test string_view_test.cc)
target_link_libraries(string_view_test ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
gtest_add_tests(TARGET string_view_test TEST_PREFIX nostd. TEST_LIST
                string_view_test)

add_executable(unique_ptr_test unique_ptr_test.cc)
target_link_libraries(unique_ptr_test ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
gtest_add_tests(TARGET unique_ptr_test TEST_PREFIX nostd. TEST_LIST
                unique_ptr_test)

add_executable(utility_test utility_test.cc)
target_link_libraries(utility_test ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
gtest_add_tests(TARGET utility_test TEST_PREFIX nostd. TEST_LIST utility_test)

add_executable(span_test span_test.cc)
target_link_libraries(span_test ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
gtest_add_tests(TARGET span_test TEST_PREFIX nostd. TEST_LIST span_test)
