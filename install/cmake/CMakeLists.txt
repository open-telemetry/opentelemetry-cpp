# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.14)
project(opentelemetry-cpp-thirdparty-install LANGUAGES CXX)

# Added in CMake 3.16. ExternalProject_Add() with GIT_SUBMODULES "" initializes
# no submodules.
if(POLICY CMP0097)
  cmake_policy(SET CMP0097 NEW)
endif()

# Added in CMake 3.19 ExternalProject step targets fully adopt their steps. This
# is required for parallel builds.
if(POLICY CMP0114)
  cmake_policy(SET CMP0114 NEW)
endif()

# This file uses CMake's ExternalProject module to fetch, build, and install all
# opentelemetry-cpp dependencies

include(ExternalProject)

# Set the third-party version tags file to read.
set(OTELCPP_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
get_filename_component(OTELCPP_SOURCE_DIR "${OTELCPP_SOURCE_DIR}" ABSOLUTE)

if(NOT OTELCPP_THIRDPARTY_FILE)
  set(OTELCPP_THIRDPARTY_FILE "${OTELCPP_SOURCE_DIR}/third_party_release")
elseif(NOT IS_ABSOLUTE OTELCPP_THIRDPARTY_FILE)
  string(PREPEND OTELCPP_THIRDPARTY_FILE "${OTELCPP_SOURCE_DIR}/")
endif()

message(
  STATUS "Reading third-party version tags from ${OTELCPP_THIRDPARTY_FILE} ...")

file(STRINGS "${OTELCPP_THIRDPARTY_FILE}" _THIRDPARTY_FILE_CONTENT)
set(_THIRDPARTY_PACKAGE_LIST "")

# Parse the third-party tags file
foreach(_raw_line IN LISTS _THIRDPARTY_FILE_CONTENT)
  string(STRIP "${_raw_line}" _line)
  if(_line STREQUAL "" OR _line MATCHES "^#")
    continue()
  endif()

  # Match "package_name=git_tag"
  if(_line MATCHES "^([^=]+)=(.+)$")
    set(_package "${CMAKE_MATCH_1}")
    set(_git_tag "${CMAKE_MATCH_2}")
    set("${_package}_GIT_TAG" "${_git_tag}")
    list(APPEND _THIRDPARTY_PACKAGE_LIST "${_package}")
    message(STATUS " - ${_package}: ${${_package}_GIT_TAG}")
  else()
    message(
      FATAL_ERROR
        "Could not parse third-party tag. Invalid line in ${OTELCPP_THIRDPARTY_FILE}. Line:\n  ${_raw_line}"
    )
  endif()
endforeach()

if(OTELCPP_THIRDPARTY_FILE STREQUAL "${OTELCPP_SOURCE_DIR}/third_party_release")
  set(USE_SUBMODULES ON)
else()
  set(USE_SUBMODULES OFF)
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_OPTIONS
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
    "-DCMAKE_CXX_STANDARD_REQUIRED=ON"
    "-DCMAKE_CXX_EXTENSIONS=OFF"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON")

message(STATUS "Installing third-party packages....")
message(STATUS "  opentelemetry-cpp_SOURCE_DIR = ${OTELCPP_SOURCE_DIR}")
message(STATUS "  third-party packages = ${_THIRDPARTY_PACKAGE_LIST}")
message(STATUS "  third-party tags file = ${OTELCPP_THIRDPARTY_FILE}")
message(STATUS "  opentelemetry-proto path = ${OTELCPP_PROTO_PATH}")
message(STATUS "  CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")
message(STATUS "  CMAKE_OPTIONS=${CMAKE_OPTIONS}")

# ------------------------------------------------------------------------
# ---- zlib ----
ExternalProject_Add(
  zlib
  STEP_TARGETS install
  GIT_REPOSITORY "https://github.com/madler/zlib.git"
  GIT_TAG ${zlib_GIT_TAG}
  GIT_SHALLOW ON
  PREFIX ${CMAKE_BINARY_DIR}/external/zlib
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  CMAKE_ARGS "${CMAKE_OPTIONS}" "-DZLIB_BUILD_EXAMPLES=OFF")

# ------------------------------------------------------------------------
# ---- curl ----
ExternalProject_Add(
  curl
  DEPENDS zlib
  STEP_TARGETS build install
  GIT_REPOSITORY "https://github.com/curl/curl.git"
  GIT_TAG ${curl_GIT_TAG}
  GIT_SHALLOW ON
  PREFIX ${CMAKE_BINARY_DIR}/external/curl
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  CMAKE_ARGS "${CMAKE_OPTIONS}"
             "-DCURL_USE_LIBPSL=OFF"
             "-DBUILD_CURL_EXE=OFF"
             "-DBUILD_LIBCURL_DOCS=OFF"
             "-DBUILD_MISC_DOCS=OFF"
             "-DENABLE_CURL_MANUAL=OFF"
             "-DBUILD_SHARED_LIBS=ON")

add_dependencies(curl-build zlib-install)
# ------------------------------------------------------------------------
# ---- abseil-cpp ----
ExternalProject_Add(
  abseil
  STEP_TARGETS install
  GIT_REPOSITORY "https://github.com/abseil/abseil-cpp.git"
  GIT_TAG ${abseil_GIT_TAG}
  GIT_SHALLOW ON
  PREFIX ${CMAKE_BINARY_DIR}/external/abseil
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  CMAKE_ARGS "${CMAKE_OPTIONS}" "-DABSL_BUILD_TESTING=OFF"
             "-DABSL_ENABLE_INSTALL=ON")

# ------------------------------------------------------------------------
# ---- protobuf ----
ExternalProject_Add(
  protobuf
  STEP_TARGETS build install
  DEPENDS zlib abseil
  GIT_REPOSITORY "https://github.com/protocolbuffers/protobuf.git"
  GIT_TAG ${protobuf_GIT_TAG}
  GIT_SHALLOW ON
  GIT_SUBMODULES ""
  PREFIX ${CMAKE_BINARY_DIR}/external/protobuf
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  CMAKE_ARGS "${CMAKE_OPTIONS}"
             "-Dprotobuf_WITH_ZLIB=ON"
             "-Dprotobuf_ZLIB_PROVIDER=package"
             "-Dprotobuf_ABSL_PROVIDER=package"
             "-Dprotobuf_INSTALL=ON"
             "-Dprotobuf_BUILD_TESTS=OFF"
             "-Dprotobuf_BUILD_EXAMPLES=OFF")

add_dependencies(protobuf-build zlib-install abseil-install)

# ------------------------------------------------------------------------
# ---- grpc ----
ExternalProject_Add(
  grpc
  DEPENDS zlib abseil protobuf
  STEP_TARGETS build
  GIT_REPOSITORY "https://github.com/grpc/grpc.git"
  GIT_TAG ${grpc_GIT_TAG}
  GIT_SHALLOW ON
  GIT_SUBMODULES "third_party/re2" "third_party/cares/cares"
                 "third_party/boringssl-with-bazel"
  PREFIX ${CMAKE_BINARY_DIR}/external/grpc
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  CMAKE_ARGS "${CMAKE_OPTIONS}"
             "-DgRPC_INSTALL=ON"
             "-DgRPC_BUILD_TESTS=OFF"
             "-DgRPC_BUILD_GRPC_CPP_PLUGIN=ON"
             "-DgRPC_BUILD_GRPC_CSHARP_PLUGIN=OFF"
             "-DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=OFF"
             "-DgRPC_BUILD_GRPC_PHP_PLUGIN=OFF"
             "-DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF"
             "-DgRPC_BUILD_GRPC_PYTHON_PLUGIN=OFF"
             "-DgRPC_BUILD_GRPC_RUBY_PLUGIN=OFF"
             "-DgRPC_BUILD_GRPCPP_OTEL_PLUGIN=OFF"
             "-DRE2_BUILD_TESTING=OFF"
             "-DgRPC_ZLIB_PROVIDER=package"
             "-DgRPC_PROTOBUF_PROVIDER=package"
             "-DgRPC_ABSL_PROVIDER=package")

add_dependencies(grpc-build zlib-install abseil-install protobuf-install)

# ------------------------------------------------------------------------
# ---- benchmark ----

set(benchmark_ARGS "")
if(USE_SUBMODULES AND EXISTS "${OTELCPP_SOURCE_DIR}/third_party/benchmark/.git")
  message(STATUS "Using submodule for benchmark")
  list(APPEND benchmark_ARGS SOURCE_DIR
       "${OTELCPP_SOURCE_DIR}/third_party/benchmark")
else()
  list(
    APPEND
    benchmark_ARGS
    SOURCE_DIR
    GIT_REPOSITORY
    "https://github.com/google/benchmark.git"
    GIT_TAG
    ${benchmark_GIT_TAG}
    GIT_SHALLOW
    ON)
endif()

ExternalProject_Add(
  benchmark
  ${benchmark_ARGS}
  PREFIX ${CMAKE_BINARY_DIR}/external/benchmark
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  CMAKE_ARGS "${CMAKE_OPTIONS}" "-DBENCHMARK_ENABLE_TESTING=OFF"
             "-DBENCHMARK_ENABLE_INSTALL=ON" "-DBUILD_SHARED_LIBS=ON")

# -----------------------------------------------------------------------
# ---- googletest ----

set(googletest_ARGS "")
if(USE_SUBMODULES AND EXISTS
                      "${OTELCPP_SOURCE_DIR}/third_party/googletest/.git")
  message(STATUS "Using submodule for googletest")
  list(APPEND googletest_ARGS SOURCE_DIR
       "${OTELCPP_SOURCE_DIR}/third_party/googletest")
else()
  list(
    APPEND
    googletest_ARGS
    GIT_REPOSITORY
    "https://github.com/google/googletest.git"
    GIT_TAG
    ${googletest_GIT_TAG}
    GIT_SHALLOW
    ON)
endif()

ExternalProject_Add(
  googletest
  ${googletest_ARGS}
  PREFIX ${CMAKE_BINARY_DIR}/external/googletest
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  CMAKE_ARGS "${CMAKE_OPTIONS}" "-DINSTALL_GTEST=ON" "-DBUILD_GMOCK=ON"
             "-DBUILD_SHARED_LIBS=ON")

# ----------------------------------------------------------------------
# ---- Microsoft.GSL ----

set(ms-gsl_ARGS "")
if(USE_SUBMODULES AND EXISTS "${OTELCPP_SOURCE_DIR}/third_party/ms-gsl/.git")
  message(STATUS "Using submodule for ms-gsl")
  list(APPEND ms-gsl_ARGS SOURCE_DIR "${OTELCPP_SOURCE_DIR}/third_party/ms-gsl")
else()
  list(
    APPEND
    ms-gsl_ARGS
    GIT_REPOSITORY
    "https://github.com/microsoft/GSL.git"
    GIT_TAG
    ${ms-gsl_GIT_TAG}
    GIT_SHALLOW
    ON)
endif()

ExternalProject_Add(
  ms-gsl
  ${ms-gsl_ARGS}
  PREFIX ${CMAKE_BINARY_DIR}/external/ms-gsl
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  CMAKE_ARGS "${CMAKE_OPTIONS}" "-DGSL_TEST=OFF" "-DGSL_INSTALL=ON")

# ----------------------------------------------------------------------
# ---- nlohmann-json ----

set(nhlohmann-json_ARGS "")
if(USE_SUBMODULES AND EXISTS
                      "${OTELCPP_SOURCE_DIR}/third_party/nlohmann-json/.git")
  message(STATUS "Using submodule for nlohmann-json")
  list(APPEND nhlohmann-json_ARGS SOURCE_DIR
       "${OTELCPP_SOURCE_DIR}/third_party/nlohmann-json")
else()
  list(
    APPEND
    nhlohmann-json_ARGS
    GIT_REPOSITORY
    "https://github.com/nlohmann/json.git"
    GIT_TAG
    ${nhlohmann-json_GIT_TAG}
    GIT_SHALLOW
    ON)
endif()

ExternalProject_Add(
  nlohmann-json
  ${nhlohmann-json_ARGS}
  PREFIX ${CMAKE_BINARY_DIR}/external/nlohmann-json
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  CMAKE_ARGS "${CMAKE_OPTIONS}" "-DJSON_BuildTests=OFF" "-DJSON_Install=ON"
             "-DJSON_MultipleHeaders=OFF")

# ----------------------------------------------------------------------
# ---- opentelemetry-proto ----
if(DEFINED OTELCPP_PROTO_PATH)
  message(
    STATUS
      "Adding opentelemetry-proto: OTELCPP_PROTO_PATH: ${OTELCPP_PROTO_PATH}")
  ExternalProject_Add(
    opentelemetry-proto
    GIT_REPOSITORY "https://github.com/open-telemetry/opentelemetry-proto.git"
    GIT_TAG ${opentelemetry-proto_GIT_TAG}
    GIT_SHALLOW ON
    SOURCE_DIR ${OTELCPP_PROTO_PATH}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND "")
endif()

# ----------------------------------------------------------------------
# ---- opentracing-cpp ----

set(opentracing-cpp_ARGS "")
if(USE_SUBMODULES AND EXISTS
                      "${OTELCPP_SOURCE_DIR}/third_party/opentracing-cpp/.git")
  message(STATUS "Using submodule for opentracing-cpp")
  list(APPEND opentracing-cpp_ARGS SOURCE_DIR
       "${OTELCPP_SOURCE_DIR}/third_party/opentracing-cpp"
       ${opentracing-cpp_ARGS})
else()
  list(
    APPEND
    opentracing-cpp_ARGS
    GIT_REPOSITORY
    "https://github.com/opentracing/opentracing-cpp.git"
    GIT_TAG
    ${opentracing-cpp_GIT_TAG}
    GIT_SHALLOW
    ON)
endif()

ExternalProject_Add(
  opentracing-cpp
  ${opentracing-cpp_ARGS}
  PREFIX ${CMAKE_BINARY_DIR}/external/opentracing-cpp
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  CMAKE_ARGS "${CMAKE_OPTIONS}" "-DBUILD_TESTING=OFF")

# ----------------------------------------------------------------------
# ---- prometheus-cpp ----

set(prometheus-cpp_ARGS "")
if(USE_SUBMODULES AND EXISTS
                      "${OTELCPP_SOURCE_DIR}/third_party/prometheus-cpp/.git")
  message(STATUS "Using submodule for prometheus-cpp")
  list(APPEND prometheus-cpp_ARGS SOURCE_DIR
       "${OTELCPP_SOURCE_DIR}/third_party/prometheus-cpp")
else()

  list(
    APPEND
    prometheus-cpp_ARGS
    GIT_REPOSITORY
    "https://github.com/jupp0r/prometheus-cpp.git"
    GIT_TAG
    "${prometheus-cpp_GIT_TAG}"
    GIT_SHALLOW
    ON
    GIT_SUBMODULES
    "3rdparty/civetweb")
endif()

ExternalProject_Add(
  prometheus-cpp
  ${prometheus-cpp_ARGS}
  PREFIX ${CMAKE_BINARY_DIR}/external/prometheus-cpp
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  STEP_TARGETS build
  DEPENDS zlib curl
  CMAKE_ARGS "${CMAKE_OPTIONS}" "-DENABLE_TESTING=OFF" "-DENABLE_PUSH=ON"
             "-DENABLE_PULL=ON")

add_dependencies(prometheus-cpp-build zlib-install curl-install)
