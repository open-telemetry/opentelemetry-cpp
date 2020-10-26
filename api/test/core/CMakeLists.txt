include(GoogleTest)

add_executable(timestamp_test timestamp_test.cc)
target_link_libraries(
  timestamp_test ${GTEST_BOTH_LIBRARIES} ${CORE_RUNTIME_LIBS}
  ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
gtest_add_tests(
  TARGET timestamp_test
  TEST_PREFIX trace.
  TEST_LIST timestamp_test)
