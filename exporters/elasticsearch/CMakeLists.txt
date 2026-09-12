# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include(${PROJECT_SOURCE_DIR}/cmake/pkgconfig.cmake)

add_library(opentelemetry_exporter_elasticsearch_logs
            src/es_log_record_exporter.cc src/es_log_recordable.cc)

set_target_properties(opentelemetry_exporter_elasticsearch_logs
                      PROPERTIES EXPORT_NAME elasticsearch_log_record_exporter)
set_target_version(opentelemetry_exporter_elasticsearch_logs)

target_include_directories(
  opentelemetry_exporter_elasticsearch_logs
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  opentelemetry_exporter_elasticsearch_logs
  PUBLIC opentelemetry_trace opentelemetry_logs opentelemetry_http_client
         nlohmann_json::nlohmann_json)

otel_add_component(
  COMPONENT
  exporters_elasticsearch
  TARGETS
  opentelemetry_exporter_elasticsearch_logs
  FILES_DIRECTORY
  "include/opentelemetry/exporters/elasticsearch"
  FILES_DESTINATION
  "include/opentelemetry/exporters"
  FILES_MATCHING
  PATTERN
  "*.h"
  PATTERN
  "es_log_recordable.h"
  EXCLUDE
  PATTERN
  "es_bulk_response.h"
  EXCLUDE
  PATTERN
  "detail"
  EXCLUDE)

if(OTELCPP_INSTALL)
  opentelemetry_add_pkgconfig(
    exporter_elasticsearch_logs "OpenTelemetry Exporter - Elasticsearch"
    "Exports log records to Elasticsearch."
    "opentelemetry_trace opentelemetry_logs")
endif()

if(OTELCPP_BUILD_TESTING)
  add_executable(es_log_record_exporter_test
                 test/es_log_record_exporter_test.cc)

  target_link_libraries(
    es_log_record_exporter_test ${GTEST_BOTH_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT} opentelemetry_exporter_elasticsearch_logs)

  gtest_add_tests(
    TARGET es_log_record_exporter_test
    TEST_PREFIX exporter.
    TEST_LIST es_log_record_exporter_test)

  # The synchronous wiring cases wait on a completion, and a regression that
  # stops delivering one hangs the job rather than reporting. A bound turns that
  # back into a red test. Not the asynchronous ones: their fake calls back from
  # inside SendRequest() and Export() returns without waiting.
  set_tests_properties(${es_log_record_exporter_test} PROPERTIES TIMEOUT 120)
endif() # OTELCPP_BUILD_TESTING
