# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Configuration Core: Builder interfaces, Configuration model, Registry, SDK
# Builder, Configured SDK
add_library(
  opentelemetry_configuration_core
  document_node.cc
  configured_sdk.cc
  sdk_builder.cc
  registry.cc
  composable_always_off_sampler_configuration.cc
  composable_always_on_sampler_configuration.cc
  composable_probability_sampler_configuration.cc
  composable_parent_threshold_sampler_configuration.cc
  composable_rule_based_sampler_configuration.cc)

set_target_properties(opentelemetry_configuration_core
                      PROPERTIES EXPORT_NAME configuration_core)

target_include_directories(
  opentelemetry_configuration_core
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  opentelemetry_configuration_core
  PUBLIC opentelemetry_api opentelemetry_common opentelemetry_trace
         opentelemetry_metrics opentelemetry_logs
  PRIVATE)

# Trace SDK component builders
add_library(opentelemetry_configuration_trace_builders trace_builders.cc)

set_target_properties(opentelemetry_configuration_trace_builders
                      PROPERTIES EXPORT_NAME configuration_trace_builders)
set_target_version(opentelemetry_configuration_trace_builders)

target_include_directories(
  opentelemetry_configuration_trace_builders
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  opentelemetry_configuration_trace_builders
  PUBLIC opentelemetry_trace
  PRIVATE opentelemetry_configuration_core)

# Logs SDK component builders
add_library(opentelemetry_configuration_logs_builders logs_builders.cc)

set_target_properties(opentelemetry_configuration_logs_builders
                      PROPERTIES EXPORT_NAME configuration_logs_builders)
set_target_version(opentelemetry_configuration_logs_builders)

target_include_directories(
  opentelemetry_configuration_logs_builders
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  opentelemetry_configuration_logs_builders
  PUBLIC opentelemetry_logs
  PRIVATE opentelemetry_configuration_core)

# Metrics SDK component builders
add_library(opentelemetry_configuration_metrics_builders metrics_builders.cc)

set_target_properties(opentelemetry_configuration_metrics_builders
                      PROPERTIES EXPORT_NAME configuration_metrics_builders)
set_target_version(opentelemetry_configuration_metrics_builders)

target_include_directories(
  opentelemetry_configuration_metrics_builders
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  opentelemetry_configuration_metrics_builders
  PUBLIC opentelemetry_metrics
  PRIVATE opentelemetry_configuration_core)

# Registry factory for to register all SDK component builders
add_library(opentelemetry_configuration_registry_factory registry_factory.cc)

set_target_properties(opentelemetry_configuration_registry_factory
                      PROPERTIES EXPORT_NAME configuration_registry_factory)
set_target_version(opentelemetry_configuration_registry_factory)

target_include_directories(
  opentelemetry_configuration_registry_factory
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  opentelemetry_configuration_registry_factory
  PUBLIC opentelemetry_configuration_core
         opentelemetry_configuration_trace_builders
         opentelemetry_configuration_metrics_builders
         opentelemetry_configuration_logs_builders)

if(OTELCPP_WITH_CONFIGURATION)

  # YAML configuration parser
  add_library(
    opentelemetry_configuration
    configuration_parser.cc yaml_configuration_parser.cc ryml_document.cc
    ryml_document_node.cc)

  set_target_properties(opentelemetry_configuration PROPERTIES EXPORT_NAME
                                                               configuration)
  set_target_version(opentelemetry_configuration)

  target_include_directories(
    opentelemetry_configuration
    PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>"
           "$<INSTALL_INTERFACE:include>")

  target_link_libraries(
    opentelemetry_configuration
    PUBLIC opentelemetry_configuration_core
    PRIVATE ryml::ryml)

  if(OPENTELEMETRY_INSTALL)
    opentelemetry_add_pkgconfig(
      configuration "OpenTelemetry SDK - Configuration"
      "Components for exporting traces in the OpenTelemetry SDK."
      "opentelemetry_configuration")
  endif()

endif(OTELCPP_WITH_CONFIGURATION)
