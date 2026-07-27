# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(
  opentelemetry_http_client_curl http_client_factory_curl.cc
                                 http_client_curl.cc http_operation_curl.cc)

set_target_properties(opentelemetry_http_client_curl
                      PROPERTIES EXPORT_NAME http_client_curl)
set_target_version(opentelemetry_http_client_curl)
target_link_libraries(opentelemetry_http_client_curl
                      PUBLIC opentelemetry_common)

# http_client_curl.h and http_operation_curl.h are installed and include
# <curl/curl.h>, and their class definitions use CURL and curl_slist, so curl is
# part of what a consumer of these headers compiles against. As a private
# dependency it was exported as a link requirement only, and a consumer whose
# curl lives outside the default include path could not compile them.
target_link_libraries(opentelemetry_http_client_curl PUBLIC opentelemetry_ext
                                                            CURL::libcurl)

if(WITH_CURL_LOGGING)
  target_compile_definitions(opentelemetry_http_client_curl
                             PRIVATE ENABLE_CURL_LOGGING)
endif()

target_compile_definitions(opentelemetry_http_client_curl
                           PUBLIC ENABLE_HTTP_CLIENT_CURL)

if(WITH_OTLP_HTTP_COMPRESSION)
  target_link_libraries(
    opentelemetry_http_client_curl
    PUBLIC opentelemetry_ext
    PRIVATE ZLIB::ZLIB)
endif()

otel_add_component(COMPONENT ext_http_curl TARGETS
                   opentelemetry_http_client_curl)
