# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  add_definitions(-DOPENTELEMETRY_BUILD_IMPORT_DLL)
endif()

include_directories(${CMAKE_SOURCE_DIR}/sdk/include)

include_directories(${CMAKE_SOURCE_DIR}/exporters/otlp/include)
include_directories(${CMAKE_SOURCE_DIR}/exporters/zipkin/include)

add_executable(example_yaml main.cc)
target_link_libraries(example_yaml ${CMAKE_THREAD_LIBS_INIT}
                      common_foo_library opentelemetry_otlp_builder
                      opentelemetry_exporter_otlp_http
                      opentelemetry_exporter_zipkin_trace
                      opentelemetry_exporter_ostream_span)

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  target_link_libraries(example_yaml opentelemetry_cpp)
else()
  target_link_libraries(example_yaml opentelemetry_configuration
opentelemetry_init
opentelemetry_common opentelemetry_trace)
endif()
