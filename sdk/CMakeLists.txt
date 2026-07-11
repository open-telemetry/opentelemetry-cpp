# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(opentelemetry_sdk INTERFACE)
target_include_directories(
  opentelemetry_sdk
  INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
            "$<INSTALL_INTERFACE:include>")

set_target_properties(opentelemetry_sdk PROPERTIES EXPORT_NAME sdk)

target_link_libraries(opentelemetry_sdk INTERFACE opentelemetry_api)

set(OTEL_SDK_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(src)

otel_add_component(
  COMPONENT
  sdk
  TARGETS
  opentelemetry_sdk
  opentelemetry_common
  opentelemetry_resources
  opentelemetry_version
  opentelemetry_logs
  opentelemetry_trace
  opentelemetry_metrics
  opentelemetry_configuration_core
  FILES_DIRECTORY
  "include/opentelemetry/"
  FILES_DESTINATION
  "include/opentelemetry"
  FILES_MATCHING
  PATTERN
  "*.h"
  PATTERN
  "sdk/configuration/configuration_parser.h"
  EXCLUDE
  PATTERN
  "sdk/configuration/yaml_configuration_parser.h"
  EXCLUDE
  PATTERN
  "sdk/configuration/ryml_document.h"
  EXCLUDE
  PATTERN
  "sdk/configuration/ryml_document_node.h"
  EXCLUDE)

if(WITH_CONFIGURATION)
  otel_add_component(
    COMPONENT
    configuration
    TARGETS
    opentelemetry_configuration
    FILES_DIRECTORY
    "include/opentelemetry/"
    FILES_DESTINATION
    "include/opentelemetry"
    FILES_MATCHING
    PATTERN
    "sdk/configuration/configuration_parser.h"
    PATTERN
    "sdk/configuration/yaml_configuration_parser.h"
    PATTERN
    "sdk/configuration/ryml_document.h"
    PATTERN
    "sdk/configuration/ryml_document_node.h")
endif()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
