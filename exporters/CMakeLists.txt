# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(OTELCPP_WITH_OTLP_GRPC
   OR OTELCPP_WITH_OTLP_HTTP
   OR OTELCPP_WITH_OTLP_FILE)
  add_subdirectory(otlp)
endif()

add_subdirectory(ostream)
add_subdirectory(memory)

if(OTELCPP_WITH_PROMETHEUS)
  add_subdirectory(prometheus)
endif()

if(OTELCPP_WITH_ZIPKIN)
  add_subdirectory(zipkin)
endif()

if(OTELCPP_WITH_ELASTICSEARCH)
  add_subdirectory(elasticsearch)
endif()

if(OTELCPP_WITH_ETW)
  add_subdirectory(etw)
endif()
