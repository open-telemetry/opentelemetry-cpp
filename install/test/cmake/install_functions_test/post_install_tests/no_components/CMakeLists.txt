# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)

project(opentelemetry-cpp-no-components-install-test LANGUAGES CXX)

# specifying no components must import all installed components
find_package(opentelemetry-cpp CONFIG REQUIRED)

if(NOT OPENTELEMETRY_CPP_FOUND)
  message(
    FATAL_ERROR
      "calling find_package with out of order components must import the components and set OPENTELEMETRY_CPP_FOUND"
  )
endif()

if(NOT opentelemetry-cpp_FOUND)
  message(
    FATAL_ERROR
      "calling find_package with out of order components must import the components and set opentelemetry-cpp_FOUND"
  )
endif()

# Test components added:
set(TEST_COMPONENT_TARGETS
    opentelemetry-cpp::test_deprecation_target
    opentelemetry-cpp::test_component_base_target
    opentelemetry-cpp::test_component_depends_target)

if(NOT TARGET opentelemetry-cpp::api)
  message(FATAL_ERROR "opentelemetry-cpp::api target not found")
endif()

if(NOT TARGET opentelemetry-cpp::sdk)
  message(FATAL_ERROR "opentelemetry-cpp::sdk target not found")
endif()

if(NOT TARGET opentelemetry-cpp::ostream_log_record_exporter)
  message(
    FATAL_ERROR
      "opentelemetry-cpp::ostream_log_record_exporter target not found")
endif()
