# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include
                    ${CMAKE_SOURCE_DIR}/ext/include ${CMAKE_SOURCE_DIR/})

add_executable(http_client client.cc)
add_executable(http_server server.cc)

target_link_libraries(
  http_client ${CMAKE_THREAD_LIBS_INIT} opentelemetry_trace
  opentelemetry_http_client_curl opentelemetry_exporter_ostream_span
  ${CURL_LIBRARIES})

target_link_libraries(
  http_server ${CMAKE_THREAD_LIBS_INIT} opentelemetry_trace
  opentelemetry_http_client_curl opentelemetry_exporter_ostream_span)
