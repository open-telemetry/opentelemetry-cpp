# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Post-install test: an unsupported/unknown component must not be found.

cmake_minimum_required(VERSION 3.16)
project(otel-install-functions-unsupported-components-test LANGUAGES NONE)

# request an unsupported/unknown component (without the REQUIRED arg)
find_package(opentelemetry-cpp CONFIG COMPONENTS test_component_base
                                                 an_unknown_component)

if(OPENTELEMETRY_CPP_FOUND)
  message(
    FATAL_ERROR
      "find_package with an unsupported component must not set OPENTELEMETRY_CPP_FOUND"
  )
endif()

if(opentelemetry-cpp_FOUND)
  message(
    FATAL_ERROR
      "find_package with an unsupported component must not set opentelemetry-cpp_FOUND"
  )
endif()

if(opentelemetry-cpp_an_unknown_component_FOUND)
  message(
    FATAL_ERROR
      "an unsupported component must not set opentelemetry-cpp_an_unknown_component_FOUND"
  )
endif()
