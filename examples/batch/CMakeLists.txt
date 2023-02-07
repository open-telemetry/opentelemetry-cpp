# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)

add_executable(batch_span_processor_example main.cc)

target_link_libraries(batch_span_processor_example ${CMAKE_THREAD_LIBS_INIT}
                      opentelemetry_exporter_ostream_span opentelemetry_trace)
