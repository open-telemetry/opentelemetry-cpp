# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)

project(opentelemetry-cpp-deprecated-components-install-test LANGUAGES CXX)

if(NOT INSTALL_TEST_DEPRECATED_COMPONENTS)
  message(FATAL_ERROR "INSTALL_TEST_DEPRECATED_COMPONENTS is empty")
endif()

message(
  STATUS
    "INSTALL_TEST_DEPRECATED_COMPONENTS: ${INSTALL_TEST_DEPRECATED_COMPONENTS}")

foreach(deprecated_component ${INSTALL_TEST_DEPRECATED_COMPONENTS})
  # if a deprecated component is a DEPRECATED_NAME of a valid component then
  # find_package must succeed.
  find_package(opentelemetry-cpp CONFIG COMPONENTS ${deprecated_component})
  if(NOT opentelemetry-cpp_FOUND)
    message(
      FATAL_ERROR
        "find_package with deprecated component '${deprecated_component}' must succeed but opentelemetry-cpp_FOUND is FALSE"
    )
  endif()
endforeach()
