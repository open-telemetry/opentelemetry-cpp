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

# The patterns below are the manifest of headers this component installs. They
# replace a "*.h" glob over include/opentelemetry/ext, so a header reaches the
# package only when it is named here, and one that exists for the build tree
# alone stays out of it.
#
# Three headers the glob used to install are deliberately absent.
# client/detail/default_factory.h is an internal helper included by exporter
# sources rather than by any public header. client/curl/http_client_curl.h and
# client/curl/http_operation_curl.h are the curl implementation, and a consumer
# creates a curl client through http_client_factory_curl.h, which depends on the
# abstract http_client_factory.h alone.
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
  "http_client.h"
  PATTERN
  "http_client_factory.h"
  PATTERN
  "http_client_factory_curl.h"
  PATTERN
  "url_parser.h"
  PATTERN
  "http_server.h"
  PATTERN
  "socket_tools.h")

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
