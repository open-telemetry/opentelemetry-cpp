include(GoogleTest)

add_executable(span_id_test span_id_test.cc)
target_link_libraries(span_id_test ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
gtest_add_tests(TARGET span_id_test TEST_PREFIX trace. TEST_LIST span_id_test)
