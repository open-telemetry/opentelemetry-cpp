# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_directories(${CMAKE_SOURCE_DIR}/exporters/zipkin/include)

add_executable(example_zipkin main.cc)
target_link_libraries(
  example_zipkin ${CMAKE_THREAD_LIBS_INIT} common_foo_library
  opentelemetry_trace opentelemetry_exporter_zipkin_trace)
