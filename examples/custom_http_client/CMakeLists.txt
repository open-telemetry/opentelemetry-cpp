# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(custom_http_client_stub STATIC custom_http_client.cc)
target_link_libraries(custom_http_client_stub PUBLIC opentelemetry_ext
                                                     opentelemetry_sdk)

target_link_libraries(opentelemetry_http_client
                      INTERFACE custom_http_client_stub)

add_executable(example_custom_http_client main.cc)
target_link_libraries(
  example_custom_http_client PRIVATE custom_http_client_stub
                                     opentelemetry-cpp::otlp_http_exporter)
