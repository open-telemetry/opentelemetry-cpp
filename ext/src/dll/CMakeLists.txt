# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set(OPENTELEMETRY_EXPORT_DEF
    "${CMAKE_CURRENT_BINARY_DIR}/opentelemetry_cpp.def")

add_library(opentelemetry_cpp SHARED dllmain.cc ${OPENTELEMETRY_EXPORT_DEF})

target_link_libraries(
  opentelemetry_cpp PRIVATE opentelemetry_trace
                            opentelemetry_exporter_ostream_span)

if(WITH_OTLP_GRPC)
  add_compile_definitions(WITH_OTLP_GRPC)
  target_link_libraries(opentelemetry_cpp
                        PRIVATE opentelemetry_exporter_otlp_grpc)
endif()

if(WITH_OTLP_HTTP)
  add_compile_definitions(WITH_OTLP_HTTP)
  target_link_libraries(opentelemetry_cpp
                        PRIVATE opentelemetry_exporter_otlp_http)
endif()

target_link_libraries(
  opentelemetry_cpp PRIVATE opentelemetry_metrics
                            opentelemetry_exporter_ostream_metrics)

if(WITH_OTLP_GRPC)
  target_link_libraries(opentelemetry_cpp
                        PRIVATE opentelemetry_exporter_otlp_grpc_metrics)
endif()

if(WITH_OTLP_HTTP)
  target_link_libraries(opentelemetry_cpp
                        PRIVATE opentelemetry_exporter_otlp_http_metric)
endif()

target_link_libraries(
  opentelemetry_cpp PRIVATE opentelemetry_logs
                            opentelemetry_exporter_ostream_logs)

if(WITH_OTLP_GRPC)
  target_link_libraries(opentelemetry_cpp
                        PRIVATE opentelemetry_exporter_otlp_grpc_log)
endif()

if(WITH_OTLP_HTTP)
  target_link_libraries(opentelemetry_cpp
                        PRIVATE opentelemetry_exporter_otlp_http_log)
endif()

add_custom_command(
  OUTPUT ${OPENTELEMETRY_EXPORT_DEF}
  COMMAND
    ${CMAKE_CXX_COMPILER}
    "-D$<JOIN:$<TARGET_PROPERTY:opentelemetry_cpp,COMPILE_DEFINITIONS>,;-D>" /EP
    ${CMAKE_CURRENT_SOURCE_DIR}/opentelemetry_cpp.src >
    ${OPENTELEMETRY_EXPORT_DEF}
  COMMAND_EXPAND_LISTS VERBATIM)

if(OPENTELEMETRY_INSTALL)
  install(
    TARGETS opentelemetry_cpp
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
