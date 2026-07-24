# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Post-install test: find_package with a deprecated component name.
#
# Requesting the deprecated name must resolve to the replacement component and
# emit a deprecation warning. The parent harness runs this project with
# -Wdeprecated and asserts the "CMake Deprecation Warning" banner with
# PASS_REGULAR_EXPRESSION.

cmake_minimum_required(VERSION 3.16)
project(otel-install-functions-deprecated-name-test LANGUAGES NONE)

find_package(opentelemetry-cpp CONFIG REQUIRED
             COMPONENTS deprecation_test_component_old)

if(NOT opentelemetry-cpp_FOUND)
  message(
    FATAL_ERROR
      "find_package with a deprecated component name must set opentelemetry-cpp_FOUND"
  )
endif()

if(NOT opentelemetry-cpp_deprecation_test_component_old_FOUND)
  message(
    FATAL_ERROR
      "the requested deprecated component name must set opentelemetry-cpp_deprecation_test_component_old_FOUND when its replacement is imported"
  )
endif()

if(NOT opentelemetry-cpp_deprecation_test_component_new_FOUND)
  message(
    FATAL_ERROR
      "the requested deprecated component name must set opentelemetry-cpp_deprecation_test_component_new_FOUND when its replacement is imported"
  )
endif()

if(NOT TARGET opentelemetry-cpp::deprecation_test_target)
  message(
    FATAL_ERROR "opentelemetry-cpp::deprecation_test_target target not imported"
  )
endif()
