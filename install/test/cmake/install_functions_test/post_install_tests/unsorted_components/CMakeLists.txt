# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Post-install test: components requested out of dependency order. Depends on
# configuration and installation of opentelemetry-cpp configured from
# install_functions_test/configure_tests/valid_components/CMakeLists.txt.
#
# test_component_depends depends on test_component_base but is listed first. The
# package config must import the components in the correct order without error.

cmake_minimum_required(VERSION 3.16)
project(otel-install-functions-unsorted-components-test LANGUAGES NONE)

find_package(
  opentelemetry-cpp CONFIG REQUIRED
  COMPONENTS test_component_depends deprecation_test_component_old
             test_component_base deprecation_test_component_new)

if(NOT opentelemetry-cpp_FOUND)
  message(
    FATAL_ERROR
      "find_package with out of order components must set opentelemetry-cpp_FOUND"
  )
endif()

if(NOT opentelemetry-cpp_test_component_depends_FOUND)
  message(
    FATAL_ERROR
      "find_package with out of order components must set opentelemetry-cpp_test_component_depends_FOUND"
  )
endif()

if(NOT opentelemetry-cpp_test_component_base_FOUND)
  message(
    FATAL_ERROR
      "find_package with out of order components must set opentelemetry-cpp_test_component_base_FOUND"
  )
endif()

if(NOT TARGET opentelemetry-cpp::test_component_base_target)
  message(
    FATAL_ERROR
      "opentelemetry-cpp::test_component_base_target target not imported")
endif()

if(NOT TARGET opentelemetry-cpp::test_component_depends_target)
  message(
    FATAL_ERROR
      "opentelemetry-cpp::test_component_depends_target target not imported")
endif()

if(NOT opentelemetry-cpp_deprecation_test_component_old_FOUND)
  message(
    FATAL_ERROR
      "find_package with out of order components must set opentelemetry-cpp_deprecation_test_component_old_FOUND"
  )
endif()

if(NOT opentelemetry-cpp_deprecation_test_component_new_FOUND)
  message(
    FATAL_ERROR
      "find_package with out of order components must set opentelemetry-cpp_deprecation_test_component_new_FOUND"
  )
endif()

if(NOT TARGET opentelemetry-cpp::deprecation_test_target)
  message(
    FATAL_ERROR "opentelemetry-cpp::deprecation_test_target target not imported"
  )
endif()
