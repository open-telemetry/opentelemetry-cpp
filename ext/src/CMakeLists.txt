# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(WITH_HTTP_CLIENT_CURL)
  add_subdirectory(http/client/curl)
endif()

if(WITH_HTTP_CLIENT_CURL OR WITH_CUSTOM_HTTP_CLIENT)
  add_library(opentelemetry_http_client INTERFACE)
  if(WITH_HTTP_CLIENT_CURL)
    target_link_libraries(opentelemetry_http_client
                          INTERFACE opentelemetry_http_client_curl)
  endif()
  # When WITH_CUSTOM_HTTP_CLIENT=ON the target is intentionally left empty; the
  # consumer populates it with their own transport implementation.
endif()

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  add_subdirectory(dll)
endif()
