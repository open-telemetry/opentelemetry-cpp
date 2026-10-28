# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Test for valid usage of the otel_add_component() CMake function. This test
# must pass the CMake configure step.

# Add a component with a deprecated name.
add_library(opentelemetry_deprecation_test_target INTERFACE)
set_target_properties(opentelemetry_deprecation_test_target
                      PROPERTIES EXPORT_NAME deprecation_test_target)
otel_add_component(
  COMPONENT deprecation_test_component_new DEPRECATED_NAMES
  deprecation_test_component_old TARGETS opentelemetry_deprecation_test_target)

# Add two components where the second depends on the first through a target
# dependency.
add_library(opentelemetry_component_dependency_test_base_target INTERFACE)
set_target_properties(opentelemetry_component_dependency_test_base_target
                      PROPERTIES EXPORT_NAME test_component_base_target)
otel_add_component(COMPONENT test_component_base TARGETS
                   opentelemetry_component_dependency_test_base_target)

add_library(opentelemetry_test_component_depends INTERFACE)
set_target_properties(opentelemetry_test_component_depends
                      PROPERTIES EXPORT_NAME test_component_depends_target)
target_link_libraries(
  opentelemetry_test_component_depends
  INTERFACE opentelemetry_component_dependency_test_base_target)
otel_add_component(COMPONENT test_component_depends TARGETS
                   opentelemetry_test_component_depends)
