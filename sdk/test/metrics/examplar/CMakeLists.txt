foreach(testname no_exemplar_reservoir_test never_sample_filter_test
                 always_sample_filter_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(${testname} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_metrics)
  gtest_add_tests(
    TARGET ${testname}
    TEST_PREFIX metrics.
    TEST_LIST ${testname})
endforeach()
