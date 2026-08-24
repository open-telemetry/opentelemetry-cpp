# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Post-install test: missing dependent components are implicitly resolved.
#
# Only test_component_depends is requested. Its dependency test_component_base
# must be imported automatically.

cmake_minimum_required(VERSION 3.16)
project(otel-install-functions-missing-components-test LANGUAGES NONE)

find_package(opentelemetry-cpp CONFIG REQUIRED
             COMPONENTS test_component_depends)

if(NOT opentelemetry-cpp_FOUND)
  message(
    FATAL_ERROR
      "find_package with a missing dependent component must set opentelemetry-cpp_FOUND"
  )
endif()

if(NOT opentelemetry-cpp_test_component_depends_FOUND)
  message(
    FATAL_ERROR
      "find_package must set opentelemetry-cpp_test_component_depends_FOUND")
endif()

if(NOT opentelemetry-cpp_test_component_base_FOUND)
  message(
    FATAL_ERROR
      "the implicitly imported dependency component must set opentelemetry-cpp_test_component_base_FOUND"
  )
endif()

if(NOT TARGET opentelemetry-cpp::test_component_depends_target)
  message(
    FATAL_ERROR
      "opentelemetry-cpp::test_component_depends_target target not imported")
endif()

if(NOT TARGET opentelemetry-cpp::test_component_base_target)
  message(
    FATAL_ERROR
      "the dependency component target opentelemetry-cpp::test_component_base_target must be imported implicitly"
  )
endif()
