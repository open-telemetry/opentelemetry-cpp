# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(common)

if(WITH_OTLP_GRPC
   OR WITH_OTLP_HTTP
   OR WITH_OTLP_FILE)
  add_subdirectory(otlp)
endif()
if(WITH_OTLP_GRPC)
  add_subdirectory(grpc)
endif()
if(WITH_ETW)
  add_subdirectory(etw_threads)
endif()
if(WITH_ZIPKIN)
  add_subdirectory(zipkin)
endif()
if(WITH_PROMETHEUS)
  add_subdirectory(prometheus)
endif()
add_subdirectory(logs_simple)
# deprecated: add_subdirectory(plugin)
add_subdirectory(simple)
add_subdirectory(batch)
add_subdirectory(metrics_simple)
add_subdirectory(multithreaded)
add_subdirectory(multi_processor)
add_subdirectory(environment_carrier)
add_subdirectory(explicit_parent)

if(WITH_EXAMPLES_HTTP)
  add_subdirectory(http)
endif()

if(WITH_CONFIGURATION)
  add_subdirectory(configuration)
endif()

# Build-tree only: depends on the opentelemetry_http_client INTERFACE target which
# is not exported in an installed package.
if(NOT WITH_HTTP_CLIENT_CURL AND WITH_OTLP_HTTP AND TARGET opentelemetry_http_client)
  add_subdirectory(custom_http_client)
endif()
