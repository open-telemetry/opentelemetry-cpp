# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_directories(include)
if(NOT TARGET prometheus-cpp::core)
  find_package(prometheus-cpp CONFIG REQUIRED)
endif()

add_library(
  opentelemetry_exporter_prometheus
  src/exporter.cc src/exporter_options.cc src/exporter_factory.cc
  src/collector.cc src/exporter_utils.cc)

set_target_properties(opentelemetry_exporter_prometheus
                      PROPERTIES EXPORT_NAME prometheus_exporter)
set_target_version(opentelemetry_exporter_prometheus)

target_include_directories(
  opentelemetry_exporter_prometheus
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

set(PROMETHEUS_EXPORTER_TARGETS opentelemetry_exporter_prometheus)
if(TARGET pull)
  list(APPEND PROMETHEUS_EXPORTER_TARGETS pull)
endif()
if(TARGET core)
  list(APPEND PROMETHEUS_EXPORTER_TARGETS core)
endif()
target_link_libraries(
  opentelemetry_exporter_prometheus
  PUBLIC opentelemetry_metrics prometheus-cpp::pull prometheus-cpp::core)

if(OPENTELEMETRY_INSTALL)
  install(
    TARGETS ${PROMETHEUS_EXPORTER_TARGETS}
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

  install(
    DIRECTORY include/opentelemetry/exporters/prometheus
    DESTINATION include/opentelemetry/exporters/
    FILES_MATCHING
    PATTERN "*.h")
endif()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
