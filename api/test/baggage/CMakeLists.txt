include(GoogleTest)

foreach(testname baggage_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(
    ${testname} ${GTEST_BOTH_LIBRARIES} ${CORE_RUNTIME_LIBS}
    ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
  gtest_add_tests(
    TARGET ${testname}
    TEST_PREFIX baggage.
    TEST_LIST ${testname})
endforeach()
add_subdirectory(propagation)
