# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# DEAD yaml_document.cc yaml_document_node.cc

add_library(
  opentelemetry_configuration
  configuration_factory.cc yaml_configuration_factory.cc ryml_document.cc
  ryml_document_node.cc)

set_target_properties(opentelemetry_configuration PROPERTIES EXPORT_NAME
                                                             configuration)
set_target_version(opentelemetry_configuration)

target_include_directories(
  opentelemetry_configuration
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>")

target_link_libraries(opentelemetry_configuration PUBLIC opentelemetry_api
                                                         opentelemetry_common)

# target_link_libraries( opentelemetry_configuration PUBLIC yaml-cpp::yaml-cpp)

target_link_libraries(opentelemetry_configuration PUBLIC ryml::ryml)

if(OPENTELEMETRY_INSTALL)
  install(
    TARGETS opentelemetry_configuration
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

  opentelemetry_add_pkgconfig(
    configuration "OpenTelemetry SDK - Configuration"
    "Components for exporting traces in the OpenTelemetry SDK."
    "opentelemetry_configuration")
endif()
