# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(
  opentelemetry_resource_detectors
  src/container_detector_utils.cc src/container_detector.cc
  src/env_entity_detector.cc src/process_detector.cc
  src/process_detector_utils.cc)

set_target_properties(opentelemetry_resource_detectors
                      PROPERTIES EXPORT_NAME resource_detectors)
set_target_version(opentelemetry_resource_detectors)

target_link_libraries(opentelemetry_resource_detectors
                      PUBLIC opentelemetry_resources)
target_include_directories(
  opentelemetry_resource_detectors
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

#
# opentelemetry_resource_detectors_builder
#

add_library(opentelemetry_resource_detectors_builder
            src/container_detector_builder.cc src/process_detector_builder.cc)

set_target_properties(opentelemetry_resource_detectors_builder
                      PROPERTIES EXPORT_NAME resource_detectors_builder)
set_target_version(opentelemetry_resource_detectors_builder)

target_include_directories(
  opentelemetry_resource_detectors_builder
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  opentelemetry_resource_detectors_builder
  PUBLIC opentelemetry_resource_detectors
  PRIVATE opentelemetry_configuration_core)

otel_add_component(
  COMPONENT
  resource_detectors
  TARGETS
  opentelemetry_resource_detectors
  FILES_DIRECTORY
  "include/opentelemetry/"
  FILES_DESTINATION
  "include/opentelemetry"
  FILES_MATCHING
  PATTERN
  "*.h"
  PATTERN
  "resource_detectors/detail/*"
  EXCLUDE
  PATTERN
  "container_detector_builder.h"
  EXCLUDE
  PATTERN
  "process_detector_builder.h"
  EXCLUDE)

otel_add_component(
  COMPONENT
  resource_detectors_builder
  TARGETS
  opentelemetry_resource_detectors_builder
  FILES_DIRECTORY
  "include/opentelemetry/"
  FILES_DESTINATION
  "include/opentelemetry"
  FILES_MATCHING
  PATTERN
  "container_detector_builder.h"
  PATTERN
  "process_detector_builder.h")

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
