# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(common)

if(OTELCPP_WITH_OTLP_GRPC
   OR OTELCPP_WITH_OTLP_HTTP
   OR OTELCPP_WITH_OTLP_FILE)
  add_subdirectory(otlp)
endif()
if(OTELCPP_WITH_OTLP_GRPC)
  add_subdirectory(grpc)
endif()
if(OTELCPP_WITH_ETW)
  add_subdirectory(etw_threads)
endif()
if(OTELCPP_WITH_ZIPKIN)
  add_subdirectory(zipkin)
endif()
if(OTELCPP_WITH_PROMETHEUS)
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
add_subdirectory(tracer_configurator)
add_subdirectory(logger_configurator)
add_subdirectory(explicit_parent)

if(OTELCPP_WITH_EXAMPLES_HTTP)
  add_subdirectory(http)
endif()

if(OTELCPP_WITH_CONFIGURATION)
  add_subdirectory(configuration)
endif()

if(OTELCPP_WITH_OTLP_HTTP)
  add_subdirectory(custom_http_client)
endif()
