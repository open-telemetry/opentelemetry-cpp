# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(WITH_HTTP_CLIENT_CURL)
  add_subdirectory(http/client/curl)
endif()

if(WITH_HTTP_CLIENT_CURL
   OR WITH_OTLP_HTTP
   OR WITH_ELASTICSEARCH
   OR WITH_ZIPKIN
   OR BUILD_W3CTRACECONTEXT_TEST
   OR WITH_EXAMPLES_HTTP)
  add_library(opentelemetry_http_client INTERFACE)
  if(WITH_HTTP_CLIENT_CURL)
    target_link_libraries(opentelemetry_http_client
                          INTERFACE opentelemetry_http_client_curl)
  endif()
endif()

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  add_subdirectory(dll)
endif()
