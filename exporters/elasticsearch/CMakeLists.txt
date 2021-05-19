include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/ext/include)

add_library(opentelemetry_exporter_elasticsearch_logs src/es_log_exporter.cc)

target_link_libraries(opentelemetry_exporter_elasticsearch_logs
                      PUBLIC opentelemetry_trace http_client_curl)

if(BUILD_TESTING)
  add_executable(es_log_exporter_test test/es_log_exporter_test.cc)

  target_link_libraries(
    es_log_exporter_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_exporter_elasticsearch_logs)

  install(
    TARGETS opentelemetry_exporter_elasticsearch_logs
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

  install(
    DIRECTORY include/opentelemetry/exporters/elasticsearch
    DESTINATION include/opentelemetry/exporters/
    FILES_MATCHING
    PATTERN "*.h")

  gtest_add_tests(
    TARGET es_log_exporter_test
    TEST_PREFIX exporter.
    TEST_LIST es_log_exporter_test)
endif() # BUILD_TESTING
