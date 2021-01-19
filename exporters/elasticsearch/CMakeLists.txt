include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/ext/include)

add_library(opentelemetry_exporter_elasticsearch_logs src/es_log_exporter.cc)

if(BUILD_TESTING)
  add_executable(es_log_exporter_test test/es_log_exporter_test.cc)

  target_link_libraries(
    es_log_exporter_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_exporter_elasticsearch_logs)

  gtest_add_tests(
    TARGET es_log_exporter_test
    TEST_PREFIX exporter.
    TEST_LIST es_log_exporter_test)
endif() # BUILD_TESTING
