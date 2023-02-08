# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include
                    ${CMAKE_SOURCE_DIR}/exporters/memory/include)

add_executable(example_multi_processor main.cc)
target_link_libraries(
  example_multi_processor ${CMAKE_THREAD_LIBS_INIT} common_foo_library
  opentelemetry_trace opentelemetry_exporter_ostream_span
  opentelemetry_exporter_in_memory)
