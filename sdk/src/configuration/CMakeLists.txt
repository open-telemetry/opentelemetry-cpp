# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(
  opentelemetry_configuration
  configuration_parser.cc
  document_node.cc
  configured_sdk.cc
  sdk_builder.cc
  registry.cc
  composable_always_off_sampler_configuration.cc
  composable_always_on_sampler_configuration.cc
  composable_probability_sampler_configuration.cc
  composable_parent_threshold_sampler_configuration.cc
  composable_rule_based_sampler_configuration.cc)

set_target_properties(opentelemetry_configuration PROPERTIES EXPORT_NAME
                                                             configuration)
set_target_version(opentelemetry_configuration)

target_include_directories(
  opentelemetry_configuration
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  opentelemetry_configuration
  PUBLIC opentelemetry_api opentelemetry_common opentelemetry_trace
         opentelemetry_metrics opentelemetry_logs)

if(OPENTELEMETRY_INSTALL)
  opentelemetry_add_pkgconfig(
    configuration "OpenTelemetry SDK - Configuration"
    "Programmatic configuration components for the OpenTelemetry SDK."
    "opentelemetry_configuration")
endif()

if(WITH_CONFIGURATION)
  add_library(
    opentelemetry_configuration_yaml
    yaml_configuration_parser.cc
    ryml_document.cc
    ryml_document_node.cc)

  set_target_properties(opentelemetry_configuration_yaml
                        PROPERTIES EXPORT_NAME configuration_yaml)
  set_target_version(opentelemetry_configuration_yaml)

  target_include_directories(
    opentelemetry_configuration_yaml
    PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>"
           "$<INSTALL_INTERFACE:include>")

  target_link_libraries(
    opentelemetry_configuration_yaml
    PUBLIC opentelemetry_configuration
    PRIVATE ryml::ryml)

  if(OPENTELEMETRY_INSTALL)
    opentelemetry_add_pkgconfig(
      configuration_yaml "OpenTelemetry SDK - YAML Configuration"
      "YAML file configuration components for the OpenTelemetry SDK."
      "opentelemetry_configuration_yaml")
  endif()
endif()
