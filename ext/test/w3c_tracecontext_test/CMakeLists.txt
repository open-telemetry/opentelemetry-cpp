# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)

add_executable(w3c_tracecontext_test main.cc)
target_link_libraries(
  w3c_tracecontext_test
  PRIVATE ${CMAKE_THREAD_LIBS_INIT} opentelemetry_trace
          opentelemetry_http_client_curl opentelemetry_exporter_ostream_span
          ${CURL_LIBRARIES} nlohmann_json::nlohmann_json)
if(nlohmann_json_clone)
  add_dependencies(w3c_tracecontext_test nlohmann_json::nlohmann_json)
endif()
