# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

#
# opentelemetry_exporter_prometheus
#

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

target_link_libraries(
  opentelemetry_exporter_prometheus
  PUBLIC opentelemetry_metrics prometheus-cpp::core
  PRIVATE prometheus-cpp::pull)

#
# opentelemetry_exporter_prometheus_builder
#

add_library(opentelemetry_exporter_prometheus_builder
            src/prometheus_pull_builder.cc)

set_target_properties(opentelemetry_exporter_prometheus_builder
                      PROPERTIES EXPORT_NAME prometheus_exporter_builder)
set_target_version(opentelemetry_exporter_prometheus_builder)

target_include_directories(
  opentelemetry_exporter_prometheus_builder
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(opentelemetry_exporter_prometheus_builder
                      PUBLIC opentelemetry_exporter_prometheus)

otel_add_component(
  COMPONENT
  exporters_prometheus
  TARGETS
  opentelemetry_exporter_prometheus
  FILES_DIRECTORY
  "include/opentelemetry/exporters/prometheus"
  FILES_DESTINATION
  "include/opentelemetry/exporters"
  FILES_MATCHING
  PATTERN
  "*.h"
  PATTERN
  "prometheus_pull_builder.h"
  EXCLUDE)

otel_add_component(
  COMPONENT
  exporters_prometheus_builder
  TARGETS
  opentelemetry_exporter_prometheus_builder
  FILES_DIRECTORY
  "include/opentelemetry/exporters/prometheus"
  FILES_DESTINATION
  "include/opentelemetry/exporters"
  FILES_MATCHING
  PATTERN
  "prometheus_pull_builder.h")

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
