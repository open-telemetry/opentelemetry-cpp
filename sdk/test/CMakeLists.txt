# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(common)
add_subdirectory(trace)
add_subdirectory(metrics)
add_subdirectory(logs)
add_subdirectory(resource)
add_subdirectory(instrumentationscope)

if(NOT OPENTELEMETRY_SKIP_DYNAMIC_LOADING_TESTS)
  add_subdirectory(singleton)
endif()
