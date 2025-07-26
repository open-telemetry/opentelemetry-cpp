# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  add_definitions(-DOPENTELEMETRY_BUILD_IMPORT_DLL)
endif()

include_directories(${CMAKE_SOURCE_DIR}/sdk/include)

add_executable(otel_configuration_check main.cc)

target_link_libraries(
  otel_configuration_check
  ${CMAKE_THREAD_LIBS_INIT}
  common_metrics_foo_library
  common_logs_foo_library
  opentelemetry_exporter_ostream_span_builder
  opentelemetry_exporter_ostream_metrics_builder
  opentelemetry_exporter_ostream_logs_builder)

if(WITH_OTLP_HTTP)
  include_directories(${CMAKE_SOURCE_DIR}/exporters/otlp/include)
  add_definitions(-DOTEL_HAVE_OTLP_HTTP)
  target_link_libraries(otel_configuration_check
                        opentelemetry_exporter_otlp_http_builder)
  target_link_libraries(otel_configuration_check
                        opentelemetry_exporter_otlp_http_log_builder)
  target_link_libraries(otel_configuration_check
                        opentelemetry_exporter_otlp_http_metric_builder)
endif()

if(WITH_OTLP_GRPC)
  include_directories(${CMAKE_SOURCE_DIR}/exporters/otlp/include)
  add_definitions(-DOTEL_HAVE_OTLP_GRPC)
  target_link_libraries(otel_configuration_check
                        opentelemetry_exporter_otlp_grpc_builder)
  target_link_libraries(otel_configuration_check
                        opentelemetry_exporter_otlp_grpc_log_builder)
  target_link_libraries(otel_configuration_check
                        opentelemetry_exporter_otlp_grpc_metrics_builder)
endif()

if(WITH_OTLP_FILE)
  include_directories(${CMAKE_SOURCE_DIR}/exporters/otlp/include)
  add_definitions(-DOTEL_HAVE_OTLP_FILE)
  target_link_libraries(otel_configuration_check
                        opentelemetry_exporter_otlp_file_builder)
  target_link_libraries(otel_configuration_check
                        opentelemetry_exporter_otlp_file_log_builder)
  target_link_libraries(otel_configuration_check
                        opentelemetry_exporter_otlp_file_metric_builder)
endif()

if(WITH_ZIPKIN)
  include_directories(${CMAKE_SOURCE_DIR}/exporters/zipkin/include)
  add_definitions(-DOTEL_HAVE_ZIPKIN)
  target_link_libraries(otel_configuration_check
                        opentelemetry_exporter_zipkin_trace_builder)
endif()

if(WITH_PROMETHEUS)
  include_directories(${CMAKE_SOURCE_DIR}/exporters/prometheus/include)
  add_definitions(-DOTEL_HAVE_PROMETHEUS)
  target_link_libraries(otel_configuration_check
                        opentelemetry_exporter_prometheus_builder)
endif()

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  target_link_libraries(otel_configuration_check opentelemetry_cpp)
else()
  target_link_libraries(
    otel_configuration_check opentelemetry_configuration opentelemetry_common
    opentelemetry_trace opentelemetry_logs)
endif()
