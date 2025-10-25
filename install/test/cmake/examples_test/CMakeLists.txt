# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)

project(opentelemetry-cpp-examples-test LANGUAGES CXX)

if(NOT DEFINED OPENTELEMETRY_CPP_EXAMPLES_SRC_DIR)
  message(
    FATAL_ERROR
      "OPENTELEMETRY_CPP_SRC_DIR must be defined when running cmake on this test project"
  )
endif()

find_package(GTest CONFIG REQUIRED)
find_package(opentelemetry-cpp CONFIG REQUIRED)

if(NOT OPENTELEMETRY_CPP_COMPONENTS_INSTALLED)
  message(
    FATAL_ERROR
      "OPENTELEMETRY_CPP_COMPONENTS_INSTALLED must be set when running cmake on this test project"
  )
endif()

if("exporters_otlp_file" IN_LIST OPENTELEMETRY_CPP_COMPONENTS_INSTALLED)
  set(WITH_OTLP_FILE ON)
endif()

if("exporters_otlp_grpc" IN_LIST OPENTELEMETRY_CPP_COMPONENTS_INSTALLED)
  set(WITH_OTLP_GRPC ON)
endif()

if("exporters_otlp_http" IN_LIST OPENTELEMETRY_CPP_COMPONENTS_INSTALLED)
  set(WITH_OTLP_HTTP ON)
endif()

if("exporters_prometheus" IN_LIST OPENTELEMETRY_CPP_COMPONENTS_INSTALLED)
  set(WITH_PROMETHEUS ON)
endif()

if("exporters_zipkin" IN_LIST OPENTELEMETRY_CPP_COMPONENTS_INSTALLED)
  set(WITH_ZIPKIN ON)
endif()

if("exporters_etw" IN_LIST OPENTELEMETRY_CPP_COMPONENTS_INSTALLED)
  set(WITH_ETW ON)
endif()

if("ext_http_curl" IN_LIST OPENTELEMETRY_CPP_COMPONENTS_INSTALLED)
  set(WITH_EXAMPLES_HTTP ON)
endif()

if("configuration" IN_LIST OPENTELEMETRY_CPP_COMPONENTS_INSTALLED)
  set(WITH_CONFIGURATION ON)
endif()

set(BUILD_TESTING ON)

include(CTest)
include(GoogleTest)

add_subdirectory(${OPENTELEMETRY_CPP_EXAMPLES_SRC_DIR}
                 ${CMAKE_CURRENT_BINARY_DIR}/examples)
