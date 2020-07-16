foreach(testname meter_test counter_aggregator_test gauge_aggregator_test
        histogram_aggregator_test min_max_sum_count_aggregator_test record_test)
    add_executable(${testname} "${testname}.cc")
    target_link_libraries(${testname} ${GTEST_BOTH_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT} opentelemetry_metrics)
    gtest_add_tests(TARGET ${testname} TEST_PREFIX metrics. TEST_LIST ${testname})
endforeach()
