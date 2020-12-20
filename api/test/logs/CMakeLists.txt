foreach(testname provider_test logger_test)
  add_executable(logs_api_${testname} "${testname}.cc")
  target_link_libraries(logs_api_${testname} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
  gtest_add_tests(
    TARGET logs_api_${testname}
    TEST_PREFIX logs.
    TEST_LIST logs_api_${testname})
endforeach()
