# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# This test covers the WITH_API_ONLY build option of opentelemetry-cpp. It
# ensures that no third-party libraries are included in the build and that only
# the opentelemetry-cpp::api target is created.

# CMake 3.24+ is required to test all targets imported globally

cmake_minimum_required(VERSION 3.24)

project(opentelemetry-cpp-api-only-test LANGUAGES CXX)

if(NOT OPENTELEMETRY_CPP_SRC_DIR)
  message(
    FATAL_ERROR
      "OPENTELEMETRY_CPP_SRC_DIR must be defined when running cmake on this test project"
  )
endif()

# This setting requires CMake 3.24 or later
set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL ON)

# Turn on the WITH_API_ONLY build option and disable GSL. This must override any
# other options and not import third party targets.
set(WITH_API_ONLY ON)
set(WITH_GSL OFF)

# Turn on all components that should be overridden by WITH_API_ONLY
set(WITH_OTLP_FILE ON)
set(WITH_OTLP_HTTP ON)
set(WITH_OTLP_GRPC ON)
set(WITH_OTLP_HTTP_COMPRESSION ON)
set(WITH_PROMETHEUS ON)
set(WITH_ZIPKIN ON)
set(WITH_ELASTICSEARCH ON)
set(WITH_OPENTRACING ON)

# Build testing off should disable importing Gtest and Benchmark targets
set(BUILD_TESTING OFF)

include(FetchContent)
FetchContent_Declare(opentelemetry-cpp SOURCE_DIR ${OPENTELEMETRY_CPP_SRC_DIR})
FetchContent_MakeAvailable(opentelemetry-cpp)

set(UNEXPECTED_TARGETS
    opentelemetry-cpp::sdk
    Microsoft.GSL::GSL
    Threads::Threads
    ZLIB::ZLIB
    CURL::libcurl
    protobuf::libprotobuf
    gRPC::grpc++
    nlohmann_json::nlohmann_json
    OpenTracing::opentracing
    OpenTracing::opentracing_static
    GTest::gtest
    benchmark::benchmark
    prometheus-cpp::core)

foreach(target IN LISTS UNEXPECTED_TARGETS)
  if(TARGET ${target})
    message(
      FATAL_ERROR
        "Unexpected target ${target} was created in an WITH_API_ONLY build.")
  endif()
endforeach()

if(NOT TARGET opentelemetry-cpp::api)
  message(
    FATAL_ERROR
      "opentelemetry-cpp::api target was not created in an WITH_API_ONLY build."
  )
endif()

FetchContent_Declare(
  googletest SOURCE_DIR ${OPENTELEMETRY_CPP_SRC_DIR}/third_party/googletest)

FetchContent_MakeAvailable(googletest)

add_executable(api_only_test
               ${OPENTELEMETRY_CPP_SRC_DIR}/install/test/src/test_api.cc)

target_link_libraries(api_only_test PRIVATE opentelemetry-cpp::api GTest::gtest
                                            GTest::gtest_main)

set(BUILD_TESTING ON)

include(CTest)
include(GoogleTest)

gtest_add_tests(
  TARGET api_only_test
  TEST_PREFIX api_only.
  TEST_LIST api_only_test)
