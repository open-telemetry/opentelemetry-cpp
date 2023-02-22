# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(WITH_OTLP)
  add_subdirectory(otlp)
endif()

add_subdirectory(ostream)
add_subdirectory(memory)

if(WITH_PROMETHEUS)
  add_subdirectory(prometheus)
endif()

if(WITH_ZIPKIN)
  add_subdirectory(zipkin)
endif()

if(WITH_ELASTICSEARCH AND WITH_LOGS_PREVIEW)
  add_subdirectory(elasticsearch)
endif()

if(WITH_ETW)
  add_subdirectory(etw)
endif()

if(WITH_JAEGER)
  add_subdirectory(jaeger)
endif()
