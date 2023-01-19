# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(opentelemetry_cpp SHARED dllmain.cc opentelemetry_cpp.def)

target_link_libraries(
  opentelemetry_cpp
  PRIVATE opentelemetry_trace
          opentelemetry_exporter_ostream_span
          opentelemetry_exporter_otlp_grpc
          opentelemetry_exporter_otlp_http
          opentelemetry_exporter_otlp_http_log
          opentelemetry_logs
          opentelemetry_exporter_otlp_grpc_log)

install(
  TARGETS opentelemetry_cpp
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
