foreach(testname
	tracez_processor_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(
    ${testname} ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_zpages opentelemetry_trace)

  gtest_add_tests(
    TARGET ${testname}
    TEST_PREFIX ext.
    TEST_LIST ${testname})
endforeach()

add_executable(tracez_processor_test tracez_processor_test.cc)
target_link_libraries(tracez_processor_test ${CMAKE_THREAD_LIBS_INIT} opentelemetry_zpages opentelemetry_trace)
add_test(tracez_processor_test)
