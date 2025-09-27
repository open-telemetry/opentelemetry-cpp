# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  add_definitions(-DOPENTELEMETRY_BUILD_IMPORT_DLL)
endif()

include_directories(${CMAKE_SOURCE_DIR}/sdk/include)

add_executable(
  example_yaml
  main.cc
  custom_sampler.cc
  custom_sampler_builder.cc
  custom_span_exporter.cc
  custom_span_exporter_builder.cc
  custom_span_processor.cc
  custom_span_processor_builder.cc
  custom_push_metric_exporter.cc
  custom_push_metric_exporter_builder.cc
  custom_pull_metric_exporter.cc
  custom_pull_metric_exporter_builder.cc
  custom_log_record_exporter.cc
  custom_log_record_exporter_builder.cc
  custom_log_record_processor.cc
  custom_log_record_processor_builder.cc)

target_link_libraries(
  example_yaml ${CMAKE_THREAD_LIBS_INIT}
  PRIVATE common_metrics_foo_library
          common_logs_foo_library
          opentelemetry-cpp::ostream_span_exporter_builder
          opentelemetry-cpp::ostream_metrics_exporter_builder
          opentelemetry-cpp::ostream_log_record_exporter_builder)

if(WITH_OTLP_HTTP)
  include_directories(${CMAKE_SOURCE_DIR}/exporters/otlp/include)
  add_definitions(-DOTEL_HAVE_OTLP_HTTP)
  target_link_libraries(example_yaml
                        PRIVATE opentelemetry-cpp::otlp_http_exporter_builder)
  target_link_libraries(
    example_yaml
    PRIVATE opentelemetry-cpp::otlp_http_log_record_exporter_builder)
  target_link_libraries(
    example_yaml PRIVATE opentelemetry-cpp::otlp_http_metric_exporter_builder)
endif()

if(WITH_OTLP_GRPC)
  include_directories(${CMAKE_SOURCE_DIR}/exporters/otlp/include)
  add_definitions(-DOTEL_HAVE_OTLP_GRPC)
  target_link_libraries(example_yaml
                        PRIVATE opentelemetry-cpp::otlp_grpc_exporter_builder)
  target_link_libraries(
    example_yaml
    PRIVATE opentelemetry-cpp::otlp_grpc_log_record_exporter_builder)
  target_link_libraries(
    example_yaml PRIVATE opentelemetry-cpp::otlp_grpc_metrics_exporter_builder)
endif()

if(WITH_OTLP_FILE)
  include_directories(${CMAKE_SOURCE_DIR}/exporters/otlp/include)
  add_definitions(-DOTEL_HAVE_OTLP_FILE)
  target_link_libraries(example_yaml
                        PRIVATE opentelemetry-cpp::otlp_file_exporter_builder)
  target_link_libraries(
    example_yaml
    PRIVATE opentelemetry-cpp::otlp_file_log_record_exporter_builder)
  target_link_libraries(
    example_yaml PRIVATE opentelemetry-cpp::otlp_file_metric_exporter_builder)
endif()

if(WITH_ZIPKIN)
  include_directories(${CMAKE_SOURCE_DIR}/exporters/zipkin/include)
  add_definitions(-DOTEL_HAVE_ZIPKIN)
  target_link_libraries(
    example_yaml PRIVATE opentelemetry-cpp::zipkin_trace_exporter_builder)
endif()

if(WITH_PROMETHEUS)
  include_directories(${CMAKE_SOURCE_DIR}/exporters/prometheus/include)
  add_definitions(-DOTEL_HAVE_PROMETHEUS)
  target_link_libraries(example_yaml
                        PRIVATE opentelemetry-cpp::prometheus_exporter_builder)
endif()

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  target_link_libraries(example_yaml
                        PRIVATE opentelemetry-cpp::opentelemetry_cpp)
else()
  target_link_libraries(
    example_yaml
    PRIVATE opentelemetry-cpp::configuration opentelemetry-cpp::common
            opentelemetry-cpp::trace opentelemetry-cpp::logs)
endif()
