# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Mock components used to test the otel install functions and the installed
# cmake package config machinery. This directory is injected into the
# opentelemetry-cpp build with OPENTELEMETRY_EXTERNAL_COMPONENT_PATH by the
# cmake.install_functions.test CI target.
#
# Each scenario is defined in its own cmake file: deprecated_name.cmake   - a
# component with a deprecated name component_depends.cmake - two components with
# an internal dependency
#
# Planned scenarios (not yet implemented): thirdparty_depends.cmake - a
# component with a third-party dependency. Requires an extension point for
# OTEL_THIRDPARTY_DEPENDENCIES_SUPPORTED so external components can register
# additional dependencies.

include(${CMAKE_CURRENT_LIST_DIR}/deprecated_name.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/component_depends.cmake)
