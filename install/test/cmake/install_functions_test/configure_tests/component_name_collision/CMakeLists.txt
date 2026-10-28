# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Tests to verify that a previously defined component cannot be marked as
# deprecated. To deprecate a component its definition must be removed and its
# name added to the DEPRECATED_NAMES of the replacement. This test must fail
# CMake the configure step.

add_library(target_a INTERFACE)
set_target_properties(target_a PROPERTIES EXPORT_NAME target_a)
otel_add_component(COMPONENT component_a TARGETS target_a)

add_library(target_b INTERFACE)
set_target_properties(target_b PROPERTIES EXPORT_NAME target_b)
otel_add_component(COMPONENT component_b TARGETS target_b DEPRECATED_NAMES
                   component_a)
