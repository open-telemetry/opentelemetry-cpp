# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

#
# opentelemetry_container_resource_detector
#

add_library(opentelemetry_container_resource_detector
            src/container_detector.cc src/container_detector_utils.cc)

set_target_properties(opentelemetry_container_resource_detector
                      PROPERTIES EXPORT_NAME container_resource_detector)
set_target_version(opentelemetry_container_resource_detector)

target_link_libraries(opentelemetry_container_resource_detector
                      PUBLIC opentelemetry_resources)
target_include_directories(
  opentelemetry_container_resource_detector
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

#
# opentelemetry_container_resource_detector_builder
#

add_library(opentelemetry_container_resource_detector_builder
            src/container_detector_builder.cc)

set_target_properties(
  opentelemetry_container_resource_detector_builder
  PROPERTIES EXPORT_NAME container_resource_detector_builder)
set_target_version(opentelemetry_container_resource_detector_builder)

target_include_directories(
  opentelemetry_container_resource_detector_builder
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  opentelemetry_container_resource_detector_builder
  PRIVATE opentelemetry_container_resource_detector
          opentelemetry_configuration_core)

#
# opentelemetry_env_entity_resource_detector
#

add_library(opentelemetry_env_entity_resource_detector
            src/env_entity_detector.cc)

set_target_properties(opentelemetry_env_entity_resource_detector
                      PROPERTIES EXPORT_NAME env_entity_resource_detector)
set_target_version(opentelemetry_env_entity_resource_detector)

target_link_libraries(opentelemetry_env_entity_resource_detector
                      PUBLIC opentelemetry_resources)
target_include_directories(
  opentelemetry_env_entity_resource_detector
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

#
# opentelemetry_host_resource_detector
#

add_library(opentelemetry_host_resource_detector src/host_detector.cc
                                                 src/host_detector_utils.cc)

set_target_properties(opentelemetry_host_resource_detector
                      PROPERTIES EXPORT_NAME host_resource_detector)
set_target_version(opentelemetry_host_resource_detector)

target_link_libraries(opentelemetry_host_resource_detector
                      PUBLIC opentelemetry_resources)
target_include_directories(
  opentelemetry_host_resource_detector
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

#
# opentelemetry_host_resource_detector_builder
#

add_library(opentelemetry_host_resource_detector_builder
            src/host_detector_builder.cc)

set_target_properties(opentelemetry_host_resource_detector_builder
                      PROPERTIES EXPORT_NAME host_resource_detector_builder)
set_target_version(opentelemetry_host_resource_detector_builder)

target_include_directories(
  opentelemetry_host_resource_detector_builder
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  opentelemetry_host_resource_detector_builder
  PRIVATE opentelemetry_host_resource_detector opentelemetry_configuration_core)

#
# opentelemetry_process_resource_detector
#

add_library(opentelemetry_process_resource_detector
            src/process_detector.cc src/process_detector_utils.cc)

set_target_properties(opentelemetry_process_resource_detector
                      PROPERTIES EXPORT_NAME process_resource_detector)
set_target_version(opentelemetry_process_resource_detector)

target_link_libraries(opentelemetry_process_resource_detector
                      PUBLIC opentelemetry_resources)
target_include_directories(
  opentelemetry_process_resource_detector
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

#
# opentelemetry_process_resource_detector_builder
#

add_library(opentelemetry_process_resource_detector_builder
            src/process_detector_builder.cc)

set_target_properties(opentelemetry_process_resource_detector_builder
                      PROPERTIES EXPORT_NAME process_resource_detector_builder)
set_target_version(opentelemetry_process_resource_detector_builder)

target_include_directories(
  opentelemetry_process_resource_detector_builder
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(
  opentelemetry_process_resource_detector_builder
  PRIVATE opentelemetry_process_resource_detector
          opentelemetry_configuration_core)

#
# opentelemetry_resource_detectors (interface to all detectors)
#

add_library(opentelemetry_resource_detectors INTERFACE)

set_target_properties(opentelemetry_resource_detectors
                      PROPERTIES EXPORT_NAME resource_detectors)

target_link_libraries(
  opentelemetry_resource_detectors
  INTERFACE opentelemetry_container_resource_detector
            opentelemetry_env_entity_resource_detector
            opentelemetry_host_resource_detector
            opentelemetry_process_resource_detector)

#
# opentelemetry_resource_detectors_builders (interface to all builders)
#

add_library(opentelemetry_resource_detectors_builders INTERFACE)

set_target_properties(opentelemetry_resource_detectors_builders
                      PROPERTIES EXPORT_NAME resource_detectors_builders)

target_link_libraries(
  opentelemetry_resource_detectors_builders
  INTERFACE opentelemetry_container_resource_detector_builder
            opentelemetry_host_resource_detector_builder
            opentelemetry_process_resource_detector_builder)

otel_add_component(
  COMPONENT
  resource_detectors
  TARGETS
  opentelemetry_resource_detectors
  opentelemetry_resource_detectors_builders
  opentelemetry_container_resource_detector
  opentelemetry_container_resource_detector_builder
  opentelemetry_env_entity_resource_detector
  opentelemetry_host_resource_detector
  opentelemetry_host_resource_detector_builder
  opentelemetry_process_resource_detector
  opentelemetry_process_resource_detector_builder
  FILES_DIRECTORY
  "include/opentelemetry/"
  FILES_DESTINATION
  "include/opentelemetry"
  FILES_MATCHING
  PATTERN
  "*.h"
  PATTERN
  "resource_detectors/detail/*"
  EXCLUDE)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
