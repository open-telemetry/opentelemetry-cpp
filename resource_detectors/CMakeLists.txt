# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(opentelemetry_resource_detectors container.cc container_detector.cc)

set_target_properties(opentelemetry_resource_detectors
                      PROPERTIES EXPORT_NAME resource_detectors)
set_target_version(opentelemetry_resource_detectors)

target_link_libraries(opentelemetry_resource_detectors
                      PUBLIC opentelemetry_common)

target_include_directories(
  opentelemetry_resource_detectors
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/resource_detectors/include>"
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>")

if(OPENTELEMETRY_INSTALL)
  opentelemetry_add_pkgconfig(
    resource_detectors "OpenTelemetry - Resource detectors"
    "Components for resource detection in the OpenTelemetry resource detectors."
  )
endif()
