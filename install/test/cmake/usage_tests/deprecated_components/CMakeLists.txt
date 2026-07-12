# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)

project(opentelemetry-cpp-deprecated-components-install-test LANGUAGES CXX)

set(DEPRECATED_COMPONENTS_WITH_ALIAS
    exporters_otlp_builder_utils exporters_otlp_file_builder
    exporters_otlp_http_builder exporters_otlp_grpc_builder
    exporters_prometheus_builder)

foreach(component ${DEPRECATED_COMPONENTS_WITH_ALIAS})
  # request a deprecated component — if it has an alias it resolves
  # transparently
  find_package(opentelemetry-cpp CONFIG COMPONENTS ${component})
  if(NOT opentelemetry-cpp_FOUND)
    message(
      FATAL_ERROR
        "find_package with deprecated component '${component}' with an alias should succeed but opentelemetry-cpp_FOUND is FALSE"
    )
  endif()
endforeach()
