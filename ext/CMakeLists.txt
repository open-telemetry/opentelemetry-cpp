# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(opentelemetry_ext INTERFACE)
target_include_directories(
  opentelemetry_ext
  INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
            "$<INSTALL_INTERFACE:include>")

set_target_properties(opentelemetry_ext PROPERTIES EXPORT_NAME "ext")
target_link_libraries(opentelemetry_ext INTERFACE opentelemetry_api)

# The embedded HTTP server headers (socket_tools.h) call Winsock, so declare the
# dependency on the interface target. MSVC also autolinks via #pragma
# comment(lib), but MinGW/GCC ignore that, so consumers there would otherwise
# fail to resolve inet_pton/WSAStartup.
if(WIN32)
  target_link_libraries(opentelemetry_ext INTERFACE ws2_32)
endif()

otel_add_component(
  COMPONENT
  ext_common
  TARGETS
  opentelemetry_ext
  FILES_DIRECTORY
  "include/opentelemetry/ext"
  FILES_DESTINATION
  "include/opentelemetry"
  FILES_MATCHING
  PATTERN
  "*.h")

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
