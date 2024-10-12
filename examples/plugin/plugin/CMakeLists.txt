# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(OPENTELEMETRY_STATIC_ONLY)
  return()
endif()

add_library(example_plugin SHARED tracer.cc factory_impl.cc)
target_link_libraries(example_plugin opentelemetry_api)
