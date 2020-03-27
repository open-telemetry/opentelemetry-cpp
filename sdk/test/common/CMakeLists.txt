foreach(testname random_test fast_random_number_generator_test)
  add_executable(${testname} "${testname}.cc")
  target_link_libraries(${testname} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_common)
  gtest_add_tests(TARGET ${testname} TEST_PREFIX trace. TEST_LIST ${testname})
endforeach()

add_executable(random_fork_test random_fork_test.cc)
target_link_libraries(random_fork_test opentelemetry_common)
add_test(random_fork_test random_fork_test)

add_executable(random_benchmark random_benchmark.cc)
target_link_libraries(random_benchmark benchmark::benchmark
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_common)
