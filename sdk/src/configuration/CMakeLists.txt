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

# TODO: Remove trace, metrics and logs library links from config core. See:
# https://github.com/open-telemetry/opentelemetry-cpp/issues/4352
target_link_libraries(
  opentelemetry_configuration_core
  PUBLIC opentelemetry_api opentelemetry_common opentelemetry_resources
         opentelemetry_trace opentelemetry_metrics opentelemetry_logs)

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
  PUBLIC opentelemetry_trace opentelemetry_configuration_core)

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
  PUBLIC opentelemetry_logs opentelemetry_configuration_core)

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
  PUBLIC opentelemetry_metrics opentelemetry_configuration_core)

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
  PRIVATE opentelemetry_configuration_trace_builders
          opentelemetry_configuration_metrics_builders
          opentelemetry_configuration_logs_builders)

if(OPENTELEMETRY_INSTALL)
  opentelemetry_add_pkgconfig(
    configuration_core
    "OpenTelemetry SDK - Configuration Core"
    "Programmatic configuration model, registry, and SDK builder."
    "opentelemetry_api"
    "opentelemetry_common"
    "opentelemetry_trace"
    "opentelemetry_metrics"
    "opentelemetry_logs"
    "opentelemetry_resources")

  opentelemetry_add_pkgconfig(
    configuration_trace_builders
    "OpenTelemetry SDK - Configuration Trace Builders"
    "Default trace SDK component builders for programmatic configuration."
    "opentelemetry_trace"
    "opentelemetry_configuration_core")

  opentelemetry_add_pkgconfig(
    configuration_logs_builders
    "OpenTelemetry SDK - Configuration Logs Builders"
    "Default logs SDK component builders for programmatic configuration."
    "opentelemetry_logs"
    "opentelemetry_configuration_core")

  opentelemetry_add_pkgconfig(
    configuration_metrics_builders
    "OpenTelemetry SDK - Configuration Metrics Builders"
    "Default metrics SDK component builders for programmatic configuration."
    "opentelemetry_metrics"
    "opentelemetry_configuration_core")

  opentelemetry_add_pkgconfig(
    configuration_registry_factory
    "OpenTelemetry SDK - Configuration Registry Factory"
    "Registry factory pre-populated with default SDK component builders."
    "opentelemetry_configuration_core"
    "opentelemetry_configuration_trace_builders"
    "opentelemetry_configuration_metrics_builders"
    "opentelemetry_configuration_logs_builders")
endif()

if(OTELCPP_WITH_CONFIGURATION)

  # YAML configuration parser
  add_library(
    opentelemetry_configuration_yaml
    configuration_parser.cc yaml_configuration_parser.cc ryml_document.cc
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
    PUBLIC opentelemetry_configuration_core
    PRIVATE ryml::ryml)

  # Full configuration interface target (links to all configuration libraries)
  # Link to this for all SDK default built-in component builders and YAML
  # parsing
  add_library(opentelemetry_configuration INTERFACE)
  set_target_properties(opentelemetry_configuration PROPERTIES EXPORT_NAME
                                                               configuration)

  set_target_version(opentelemetry_configuration)
  target_link_libraries(
    opentelemetry_configuration
    INTERFACE opentelemetry_configuration_core
              opentelemetry_configuration_yaml
              opentelemetry_configuration_registry_factory
              opentelemetry_configuration_trace_builders
              opentelemetry_configuration_metrics_builders
              opentelemetry_configuration_logs_builders)

  if(OPENTELEMETRY_INSTALL)
    opentelemetry_add_pkgconfig(
      configuration_yaml "OpenTelemetry SDK - Configuration YAML"
      "YAML configuration parser for the OpenTelemetry SDK."
      "opentelemetry_configuration_core")

    opentelemetry_add_pkgconfig(
      configuration
      "OpenTelemetry SDK - Configuration"
      "Full YAML configuration interface: parser, registry, and all default builders."
      "opentelemetry_configuration_core"
      "opentelemetry_configuration_yaml"
      "opentelemetry_configuration_registry_factory"
      "opentelemetry_configuration_trace_builders"
      "opentelemetry_configuration_metrics_builders"
      "opentelemetry_configuration_logs_builders")
  endif()

endif(OTELCPP_WITH_CONFIGURATION)
