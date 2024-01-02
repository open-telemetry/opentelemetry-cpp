# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(common_foo_library foo_library.h foo_library.cc)
set_target_version(common_foo_library)

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  message(STATUS "OPENTELEMETRY_BUILD_DLL for common_foo_library")
  target_compile_definitions(common_foo_library PRIVATE OPENTELEMETRY_BUILD_IMPORT_DLL=1)
  target_link_libraries(common_foo_library opentelemetry_cpp)
else()
  target_link_libraries(common_foo_library PUBLIC ${CMAKE_THREAD_LIBS_INIT}
                                                  opentelemetry_api)
endif()
