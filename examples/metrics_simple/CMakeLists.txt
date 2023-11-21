# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  add_definitions(-DOPENTELEMETRY_BUILD_IMPORT_DLL)
endif()

include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)

add_executable(metrics_ostream_example metrics_ostream.cc)
target_link_libraries(metrics_ostream_example ${CMAKE_THREAD_LIBS_INIT}
                      common_metrics_foo_library)

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  target_link_libraries(metrics_ostream_example opentelemetry_cpp)
else()
  target_link_libraries(
    metrics_ostream_example opentelemetry_metrics
    opentelemetry_exporter_ostream_metrics opentelemetry_resources)
endif()
