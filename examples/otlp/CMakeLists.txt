# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  add_definitions(-DOPENTELEMETRY_BUILD_IMPORT_DLL)
endif()

include_directories(
  ${CMAKE_BINARY_DIR}/generated/third_party/opentelemetry-proto
  ${CMAKE_SOURCE_DIR}/exporters/otlp/include)

if(WITH_OTLP_GRPC)
  # TRACE

  add_executable(example_otlp_grpc grpc_main.cc)

  target_link_libraries(example_otlp_grpc ${CMAKE_THREAD_LIBS_INIT}
                        common_foo_library)

  if(DEFINED OPENTELEMETRY_BUILD_DLL)
    target_link_libraries(example_otlp_grpc opentelemetry_cpp)
  else()
    target_link_libraries(example_otlp_grpc opentelemetry_trace
                          opentelemetry_exporter_otlp_grpc)
  endif()

  # METRIC

  add_executable(example_otlp_grpc_metric grpc_metric_main.cc)

  target_link_libraries(example_otlp_grpc_metric ${CMAKE_THREAD_LIBS_INIT}
                        common_metrics_foo_library)

  if(DEFINED OPENTELEMETRY_BUILD_DLL)
    target_link_libraries(example_otlp_grpc_metric opentelemetry_cpp)
  else()
    target_link_libraries(example_otlp_grpc_metric opentelemetry_metrics
                          opentelemetry_exporter_otlp_grpc_metrics)
  endif()

  # LOG

  add_executable(example_otlp_grpc_log grpc_log_main.cc)

  target_link_libraries(example_otlp_grpc_log ${CMAKE_THREAD_LIBS_INIT}
                        common_logs_foo_library)
  if(DEFINED OPENTELEMETRY_BUILD_DLL)
    target_link_libraries(example_otlp_grpc_log opentelemetry_cpp)
  else()
    target_link_libraries(
      example_otlp_grpc_log opentelemetry_trace opentelemetry_logs
      opentelemetry_exporter_otlp_grpc opentelemetry_exporter_otlp_grpc_log)
  endif()
endif()

if(WITH_OTLP_HTTP)
  # TRACE

  add_executable(example_otlp_http http_main.cc)

  target_link_libraries(example_otlp_http ${CMAKE_THREAD_LIBS_INIT}
                        common_foo_library)
  if(DEFINED OPENTELEMETRY_BUILD_DLL)
    target_link_libraries(example_otlp_http opentelemetry_cpp
                          opentelemetry_common)
  else()
    target_link_libraries(example_otlp_http opentelemetry_trace
                          opentelemetry_exporter_otlp_http)
  endif()

  # METRIC

  add_executable(example_otlp_http_metric http_metric_main.cc)
  target_link_libraries(example_otlp_http_metric ${CMAKE_THREAD_LIBS_INIT}
                        common_metrics_foo_library)

  if(DEFINED OPENTELEMETRY_BUILD_DLL)
    target_link_libraries(example_otlp_http_metric opentelemetry_cpp
                          opentelemetry_common)
  else()
    target_link_libraries(
      example_otlp_http_metric common_metrics_foo_library opentelemetry_metrics
      opentelemetry_exporter_otlp_http_metric)
  endif()

  # LOG

  add_executable(example_otlp_http_log http_log_main.cc)
  target_link_libraries(example_otlp_http_log ${CMAKE_THREAD_LIBS_INIT}
                        common_logs_foo_library)

  if(DEFINED OPENTELEMETRY_BUILD_DLL)
    target_link_libraries(example_otlp_http_log opentelemetry_cpp
                          opentelemetry_common)
  else()
    target_link_libraries(
      example_otlp_http_log common_logs_foo_library opentelemetry_trace
      opentelemetry_logs opentelemetry_exporter_otlp_http
      opentelemetry_exporter_otlp_http_log)
  endif()

endif()
