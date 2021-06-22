include(GoogleTest)

foreach(testname instrumentationlibrary_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(${testname} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
  gtest_add_tests(
    TARGET ${testname}
    TEST_PREFIX instrumentationlibrary.
    TEST_LIST ${testname})
endforeach()
