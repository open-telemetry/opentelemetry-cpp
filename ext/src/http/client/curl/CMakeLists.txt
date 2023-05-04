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
if(TARGET CURL::libcurl)
  target_link_libraries(
    opentelemetry_http_client_curl
    PUBLIC opentelemetry_ext
    PRIVATE CURL::libcurl)
else()
  target_include_directories(opentelemetry_http_client_curl
                             INTERFACE "${CURL_INCLUDE_DIRS}")
  target_link_libraries(
    opentelemetry_http_client_curl
    PUBLIC opentelemetry_ext
    PRIVATE ${CURL_LIBRARIES})
endif()

if(OPENTELEMETRY_INSTALL)
  install(
    TARGETS opentelemetry_http_client_curl
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
