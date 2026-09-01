# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(OTELCPP_WITH_HTTP_CLIENT_CURL)
  add_subdirectory(http/client/curl)
endif()

if(OTELCPP_WITH_HTTP_CLIENT_CURL
   OR OTELCPP_WITH_OTLP_HTTP
   OR OTELCPP_WITH_ELASTICSEARCH
   OR OTELCPP_WITH_ZIPKIN
   OR OTELCPP_BUILD_W3CTRACECONTEXT_TEST
   OR OTELCPP_WITH_EXAMPLES_HTTP)
  add_library(opentelemetry_http_client INTERFACE)
  target_link_libraries(opentelemetry_http_client INTERFACE opentelemetry_ext)
  if(OTELCPP_WITH_HTTP_CLIENT_CURL)
    target_link_libraries(opentelemetry_http_client
                          INTERFACE opentelemetry_http_client_curl)
  endif()

  set_target_properties(opentelemetry_http_client PROPERTIES EXPORT_NAME
                                                             http_client)
  otel_add_component(COMPONENT ext_http TARGETS opentelemetry_http_client)
endif()

if(OTELCPP_BUILD_DLL)
  add_subdirectory(dll)
endif()
