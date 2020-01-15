include(GoogleTest)

add_executable(noop_test noop_test.cc)
target_link_libraries(noop_test ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
gtest_add_tests(TARGET noop_test TEST_PREFIX trace. TEST_LIST noop_test)
