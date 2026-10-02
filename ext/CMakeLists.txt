# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(opentelemetry_ext INTERFACE)
target_include_directories(
  opentelemetry_ext
  INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
            "$<INSTALL_INTERFACE:include>")

set_target_properties(opentelemetry_ext PROPERTIES EXPORT_NAME "ext")
target_link_libraries(opentelemetry_ext INTERFACE opentelemetry_api)

# client/detail/default_factory.h is an internal helper included by exporter
# sources rather than by any public header. client/curl/http_client_curl.h and
# client/curl/http_operation_curl.h are the curl implementation, reached through
# http_client_factory_curl.h, which depends on the abstract
# http_client_factory.h alone. server/http_server.h and server/socket_tools.h
# are the embedded server used by tests and the http example.
#
# The two directory excludes are what keeps client/detail and server out of the
# install tree entirely: install(DIRECTORY) creates a destination directory for
# every source directory it walks, so filtering on file names alone would leave
# both behind, empty.
#
# TODO: the embedded server headers do not belong beside the user facing ones
# under ext/include. Move them to an internal location such as test_common, see
# #4332.
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
  "detail"
  EXCLUDE
  PATTERN
  "server"
  EXCLUDE)

add_subdirectory(src)

if(OTELCPP_BUILD_TESTING)
  add_subdirectory(test)
endif()
