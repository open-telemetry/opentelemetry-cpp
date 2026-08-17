# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Test to verify that duplicate deprecated component names are not allowed. This
# test must fail the CMake configure step.

add_library(target_a INTERFACE)
set_target_properties(target_a PROPERTIES EXPORT_NAME target_a)
otel_add_component(COMPONENT component_a TARGETS target_a DEPRECATED_NAMES
                   old_name)

add_library(target_b INTERFACE)
set_target_properties(target_b PROPERTIES EXPORT_NAME target_b)
otel_add_component(COMPONENT component_b TARGETS target_b DEPRECATED_NAMES
                   old_name)
