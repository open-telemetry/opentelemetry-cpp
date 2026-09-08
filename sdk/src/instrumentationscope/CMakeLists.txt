# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(opentelemetry_instrumentation_scope instrumentation_scope.cc)

set_target_properties(opentelemetry_instrumentation_scope
                      PROPERTIES EXPORT_NAME instrumentation_scope)
set_target_version(opentelemetry_instrumentation_scope)

target_include_directories(
  opentelemetry_instrumentation_scope
  PUBLIC "$<BUILD_INTERFACE:${OTEL_SDK_DIR}>" "$<INSTALL_INTERFACE:include>")

target_link_libraries(opentelemetry_instrumentation_scope
                      PUBLIC opentelemetry_common)

if(OPENTELEMETRY_INSTALL)
  opentelemetry_add_pkgconfig(
    instrumentation_scope
    "OpenTelemetry SDK - Instrumentation Scope"
    "Instrumentation Scope components for the OpenTelemetry SDK, a library for exporting telemetry."
    "opentelemetry_common")
endif()
