# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  add_definitions(-DOPENTELEMETRY_BUILD_IMPORT_DLL)
endif()

include_directories(${CMAKE_SOURCE_DIR}/sdk/include)

add_executable(example_yaml main.cc
  custom_sampler.cc custom_sampler_builder.cc
  custom_span_exporter.cc custom_span_exporter_builder.cc
  custom_span_processor.cc custom_span_processor_builder.cc)

target_link_libraries(example_yaml ${CMAKE_THREAD_LIBS_INIT}
                      common_foo_library
                      opentelemetry_exporter_ostream_span)

if(WITH_OTLP_HTTP)
  include_directories(${CMAKE_SOURCE_DIR}/exporters/otlp/include)
  add_definitions(-DOTEL_HAVE_OTLP)
  target_link_libraries(example_yaml opentelemetry_otlp_builder)
endif()

if(WITH_ZIPKIN)
  include_directories(${CMAKE_SOURCE_DIR}/exporters/zipkin/include)
  add_definitions(-DOTEL_HAVE_ZIPKIN)
  target_link_libraries(example_yaml opentelemetry_exporter_zipkin_trace)
endif()


if(DEFINED OPENTELEMETRY_BUILD_DLL)
  target_link_libraries(example_yaml opentelemetry_cpp)
else()
  target_link_libraries(example_yaml opentelemetry_configuration
opentelemetry_init
opentelemetry_common opentelemetry_trace)
endif()
