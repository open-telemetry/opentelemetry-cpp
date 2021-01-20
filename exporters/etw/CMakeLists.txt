include_directories(include)

if(BUILD_TESTING)
  add_executable(etw_provider_test test/etw_provider_test.cc)
  add_executable(etw_tracer_test test/etw_tracer_test.cc)

  target_link_libraries(etw_provider_test ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT})

  target_link_libraries(etw_tracer_test ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT})

  gtest_add_tests(
    TARGET etw_provider_test
    TEST_PREFIX exporter.
    TEST_LIST etw_provider_test)
  gtest_add_tests(
    TARGET etw_tracer_test
    TEST_PREFIX exporter.
    TEST_LIST etw_tracer_test)
endif() # BUILD_TESTING
