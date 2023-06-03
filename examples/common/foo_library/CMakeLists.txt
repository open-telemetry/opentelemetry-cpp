# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  add_definitions(-DOPENTELEMETRY_BUILD_IMPORT_DLL)
endif()

add_library(common_foo_library foo_library.h foo_library.cc)
set_target_version(common_foo_library)

target_link_libraries(common_foo_library PUBLIC ${CMAKE_THREAD_LIBS_INIT}
                                                opentelemetry_api)
