# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(
  opentelemetry_init
  configured_sdk.cc sdk_builder.cc registry.cc)

set_target_properties(opentelemetry_init PROPERTIES EXPORT_NAME init)
set_target_version(opentelemetry_init)

target_include_directories(
  opentelemetry_init
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>")

target_link_libraries(
  opentelemetry_init
  PUBLIC opentelemetry_configuration opentelemetry_trace)

if(OPENTELEMETRY_INSTALL)
  install(
    TARGETS opentelemetry_init
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

  opentelemetry_add_pkgconfig(
    init "OpenTelemetry SDK - Init"
    "Components for exporting traces in the OpenTelemetry SDK."
    "opentelemetry_init")
endif()
