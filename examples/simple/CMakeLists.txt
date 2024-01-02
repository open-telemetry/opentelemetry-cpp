# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)

add_executable(example_simple main.cc)
target_link_libraries(example_simple ${CMAKE_THREAD_LIBS_INIT}
                      common_foo_library)

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  message(STATUS "OPENTELEMETRY_BUILD_DLL for example_simple")
  
  target_compile_definitions(example_simple PRIVATE OPENTELEMETRY_BUILD_IMPORT_DLL)
  
  target_link_libraries(example_simple opentelemetry_cpp)
else()
  target_link_libraries(example_simple opentelemetry_trace
                        opentelemetry_exporter_ostream_span)
endif()
