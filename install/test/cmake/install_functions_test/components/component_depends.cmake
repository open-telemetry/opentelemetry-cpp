# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Scenario: two components with an internal component dependency.
#
# COMPONENT test_component_depends links the target of
# COMPONENT test_component_base. The install functions must record the
# dependency so that consumers requesting only test_component_depends also
# get test_component_base imported automatically and in the correct order.

add_library(opentelemetry_component_dependency_test_base_target INTERFACE)
set_target_properties(opentelemetry_component_dependency_test_base_target
                      PROPERTIES EXPORT_NAME test_component_base_target)

add_library(opentelemetry_test_component_depends INTERFACE)
set_target_properties(opentelemetry_test_component_depends
                      PROPERTIES EXPORT_NAME test_component_depends_target)
target_link_libraries(opentelemetry_test_component_depends
                      INTERFACE opentelemetry_component_dependency_test_base_target)

# Components must be added in dependency order (base first) so the dependency
# lookup in otel_add_component can resolve the base component.
otel_add_component(COMPONENT test_component_base TARGETS
                   opentelemetry_component_dependency_test_base_target)

otel_add_component(COMPONENT test_component_depends TARGETS
                   opentelemetry_test_component_depends)
