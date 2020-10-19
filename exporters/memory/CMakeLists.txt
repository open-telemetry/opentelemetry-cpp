include_directories(include)

add_library(opentelemetry_exporter_in_memory INTERFACE)

target_include_directories(opentelemetry_exporter_in_memory INTERFACE include/)

if(BUILD_TESTING)
  add_executable(in_memory_span_data_test test/in_memory_span_data_test.cc)
  add_executable(in_memory_span_exporter_test
                 test/in_memory_span_exporter_test.cc)

  target_link_libraries(
    in_memory_span_data_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_exporter_in_memory)

  target_link_libraries(
    in_memory_span_exporter_test ${GTEST_BOTH_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT} opentelemetry_exporter_in_memory)

  gtest_add_tests(TARGET in_memory_span_data_test TEST_PREFIX exporter.
                  TEST_LIST in_memory_span_data_test)
  gtest_add_tests(TARGET in_memory_span_exporter_test TEST_PREFIX exporter.
                  TEST_LIST in_memory_span_exporter_test)
endif()
