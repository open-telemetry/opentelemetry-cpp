add_library(opentelemetry_exporter_in_memory INTERFACE)

target_include_directories(
  opentelemetry_exporter_in_memory
  INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
            "$<INSTALL_INTERFACE:include>")

set_target_properties(opentelemetry_exporter_in_memory
                      PROPERTIES EXPORT_NAME in_memory_span_exporter)

install(
  TARGETS opentelemetry_exporter_in_memory
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY include/opentelemetry/exporters/memory
  DESTINATION include/opentelemetry/exporters
  FILES_MATCHING
  PATTERN "*.h")

if(BUILD_TESTING)
  add_executable(in_memory_span_data_test test/in_memory_span_data_test.cc)
  add_executable(in_memory_span_exporter_test
                 test/in_memory_span_exporter_test.cc)

  target_link_libraries(
    in_memory_span_data_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_exporter_in_memory opentelemetry_resources)

  target_link_libraries(
    in_memory_span_exporter_test ${GTEST_BOTH_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT} opentelemetry_exporter_in_memory
    opentelemetry_resources)

  gtest_add_tests(
    TARGET in_memory_span_data_test
    TEST_PREFIX exporter.
    TEST_LIST in_memory_span_data_test)
  gtest_add_tests(
    TARGET in_memory_span_exporter_test
    TEST_PREFIX exporter.
    TEST_LIST in_memory_span_exporter_test)
endif()
