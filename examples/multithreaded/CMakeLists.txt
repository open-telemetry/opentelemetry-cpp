# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)

add_executable(example_multithreaded main.cc)
target_link_libraries(example_multithreaded ${CMAKE_THREAD_LIBS_INIT}
                      opentelemetry_trace opentelemetry_exporter_ostream_span)
