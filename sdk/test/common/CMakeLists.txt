foreach(testname atomic_unique_ptr_test circular_buffer_range_test circular_buffer_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(${testname} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_trace)
  gtest_add_tests(TARGET ${testname} TEST_PREFIX trace. TEST_LIST ${testname})
endforeach()

add_executable(circular_buffer_benchmark circular_buffer_benchmark.cc)
target_link_libraries(circular_buffer_benchmark benchmark::benchmark
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
