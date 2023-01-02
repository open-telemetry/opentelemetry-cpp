set(this_target shim_test)

file(GLOB source_files ${CMAKE_CURRENT_LIST_DIR}/*.*)

add_executable(${this_target} ${source_files})

target_link_libraries(${this_target} 
    ${GTEST_BOTH_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT} 
    opentelemetry_api
    opentelemetry_opentracing_shim
    opentracing)

#gtest_add_tests(
#    TARGET ${this_target}
#    TEST_PREFIX ${this_target}.
#    TEST_LIST ${this_target})
