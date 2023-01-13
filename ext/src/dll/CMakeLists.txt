# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(opentelemetry_cpp SHARED dllmain.cc opentelemetry_cpp.def)

target_link_libraries(opentelemetry_cpp PRIVATE opentelemetry_trace opentelemetry_exporter_ostream_span)
