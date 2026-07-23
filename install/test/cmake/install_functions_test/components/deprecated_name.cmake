# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Scenario: a component with a deprecated name.
#
# COMPONENT test_component_deprecated is the replacement for the deprecated
# component name test_component_deprecated_v1. Consumers requesting the
# deprecated name via find_package(... COMPONENTS test_component_deprecated_v1)
# must receive the replacement component and a deprecation warning.

add_library(opentelemetry_deprecation_test_target INTERFACE)
set_target_properties(opentelemetry_deprecation_test_target
                      PROPERTIES EXPORT_NAME deprecation_test_target)

otel_add_component(
  COMPONENT deprecation_test_component_new
  DEPRECATED_NAMES deprecation_test_component_old
  TARGETS opentelemetry_deprecation_test_target)
