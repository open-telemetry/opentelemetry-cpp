# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_directories(${CMAKE_SOURCE_DIR}/exporters/otlp/include)

if(WITH_OTLP_HTTP)
  add_executable(func_otlp_http func_http_main.cc)
  target_link_libraries(func_otlp_http ${CMAKE_THREAD_LIBS_INIT}
                        opentelemetry_trace opentelemetry_exporter_otlp_http)
endif()
