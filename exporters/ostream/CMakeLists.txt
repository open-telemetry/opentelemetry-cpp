add_library(opentelemetry_exporter_ostream_span src/span_exporter.cc)

set_target_properties(opentelemetry_exporter_ostream_span
                      PROPERTIES EXPORT_NAME ostream_span_exporter)

target_include_directories(
  opentelemetry_exporter_ostream_span
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>")

target_link_libraries(opentelemetry_exporter_ostream_span
                      PUBLIC opentelemetry_trace)

install(
  TARGETS opentelemetry_exporter_ostream_span
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY include/opentelemetry/exporters/ostream
  DESTINATION include/opentelemetry/exporters
  PATTERN "*.h"
  PATTERN "metrics_exporter.h" EXCLUDE
  PATTERN "log_Exporter.h" EXCLUDE)

if(BUILD_TESTING)
  add_executable(ostream_span_test test/ostream_span_test.cc)
  target_link_libraries(ostream_span_test ${GTEST_BOTH_LIBRARIES}
                        opentelemetry_exporter_ostream_span)
  gtest_add_tests(
    TARGET ostream_span_test
    TEST_PREFIX exporter.
    TEST_LIST ostream_span_test)
endif() # BUILD_TESTING

if(WITH_METRICS_PREVIEW)
  add_library(opentelemetry_exporter_ostream_metrics_deprecated
              src/metrics_exporter.cc)
  set_target_properties(opentelemetry_exporter_ostream_metrics_deprecated
                        PROPERTIES EXPORT_NAME ostream_metrics_exporter)
  target_include_directories(
    opentelemetry_exporter_ostream_metrics_deprecated
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>")
  target_link_libraries(opentelemetry_exporter_ostream_metrics_deprecated
                        PUBLIC opentelemetry_metrics_deprecated)
  install(
    TARGETS opentelemetry_exporter_ostream_metrics_deprecated
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(
    DIRECTORY include/opentelemetry/exporters/ostream
    DESTINATION include/opentelemetry/exporters
    PATTERN "metrics_exporter.h")
  if(BUILD_TESTING)
    add_executable(ostream_metrics_test test/ostream_metrics_test.cc)
    target_link_libraries(ostream_metrics_test ${GTEST_BOTH_LIBRARIES}
                          opentelemetry_exporter_ostream_metrics_deprecated)
    gtest_add_tests(
      TARGET ostream_metrics_test
      TEST_PREFIX exporter.
      TEST_LIST ostream_metrics_test)
  endif()
else()
  add_library(opentelemetry_exporter_ostream_metrics src/metric_exporter.cc)
  set_target_properties(opentelemetry_exporter_ostream_metrics
                        PROPERTIES EXPORT_NAME ostream_metrics_exporter)
  target_include_directories(
    opentelemetry_exporter_ostream_metrics
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>")
  target_link_libraries(opentelemetry_exporter_ostream_metrics
                        PUBLIC opentelemetry_metrics)
  install(
    TARGETS opentelemetry_exporter_ostream_metrics
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(
    DIRECTORY include/opentelemetry/exporters/ostream
    DESTINATION include/opentelemetry/exporters
    PATTERN "metric_exporter.h")
  if(BUILD_TESTING)
    add_executable(ostream_metric_test test/ostream_metric_test.cc)
    target_link_libraries(
      ostream_metric_test ${GTEST_BOTH_LIBRARIES}
      opentelemetry_exporter_ostream_metrics opentelemetry_resources)
    gtest_add_tests(
      TARGET ostream_metric_test
      TEST_PREFIX exporter.
      TEST_LIST ostream_metric_test)
  endif()
endif()

if(WITH_LOGS_PREVIEW)
  add_library(opentelemetry_exporter_ostream_logs src/log_exporter.cc)
  set_target_properties(opentelemetry_exporter_ostream_logs
                        PROPERTIES EXPORT_NAME ostream_log_exporter)
  target_include_directories(
    opentelemetry_exporter_ostream_logs
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>")
  target_link_libraries(opentelemetry_exporter_ostream_logs
                        PUBLIC opentelemetry_logs)
  install(
    TARGETS opentelemetry_exporter_ostream_logs
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(
    DIRECTORY include/opentelemetry/exporters/ostream
    DESTINATION include/opentelemetry/exporters
    PATTERN "log_exporter.h")
  if(BUILD_TESTING)
    add_executable(ostream_log_test test/ostream_log_test.cc)
    target_link_libraries(ostream_log_test ${GTEST_BOTH_LIBRARIES}
                          opentelemetry_exporter_ostream_logs)
    gtest_add_tests(
      TARGET ostream_log_test
      TEST_PREFIX exporter.
      TEST_LIST ostream_log_test)
  endif()
endif()
