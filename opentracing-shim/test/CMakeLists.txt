set(this_target shim_test)

add_executable(${this_target} "${this_target}.cc")
target_link_libraries(${this_target} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
gtest_add_tests(
    TARGET ${this_target}
    TEST_PREFIX ${this_target}.
    TEST_LIST ${this_target})
