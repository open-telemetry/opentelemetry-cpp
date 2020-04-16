foreach(testname dispatcher_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(${testname} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_event_libevent)
  gtest_add_tests(TARGET ${testname} TEST_PREFIX event. TEST_LIST ${testname})
endforeach()
