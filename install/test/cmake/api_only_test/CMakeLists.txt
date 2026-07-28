# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# This test covers the WITH_API_ONLY build option of opentelemetry-cpp. It
# ensures that no third-party libraries are included in the build and that only
# the opentelemetry-cpp::api target is created.

cmake_minimum_required(VERSION 3.16)

project(opentelemetry-cpp-api-only-test LANGUAGES CXX)

if(NOT OPENTELEMETRY_CPP_SRC_DIR)
  message(
    FATAL_ERROR
      "OPENTELEMETRY_CPP_SRC_DIR must be defined when running cmake on this test project"
  )
endif()

set(EXPECTED_OPTIONS_IN_CACHE
    WITH_EXAMPLES
    WITH_EXAMPLES_HTTP
    BUILD_TESTING
    WITH_FUNC_TESTS
    WITH_BENCHMARK
    WITH_OTLP_GRPC
    WITH_OTLP_HTTP
    WITH_OTLP_FILE
    WITH_OTLP_HTTP_COMPRESSION
    WITH_PROMETHEUS
    WITH_ZIPKIN
    WITH_ELASTICSEARCH
    WITH_OPENTRACING
    WITH_CONFIGURATION)

function(check_options_in_cache)
  foreach(component IN LISTS EXPECTED_OPTIONS_IN_CACHE)
    if(NOT ${component})
      message(
        FATAL_ERROR
          "Expected component ${component} to be ON in the cache, but it is OFF."
      )
    endif()
  endforeach()
endfunction()

# Verify that all options which may bring in a third-party dependency are ON in
# the cache.
check_options_in_cache()

# Turn on the WITH_API_ONLY build option and disable GSL. This must override any
# other options and not import third party targets.
set(WITH_API_ONLY ON)
set(WITH_GSL OFF)

# Add the opentelemetry-cpp source directory as a subdirectory to mimic
# vendoring the library.
add_subdirectory(${OPENTELEMETRY_CPP_SRC_DIR}
                 ${CMAKE_BINARY_DIR}/opentelemetry-cpp)

# Verify that all options which may bring in a third-party dependency are still
# ON in the cache.
check_options_in_cache()

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
    prometheus-cpp::core
    ryml::ryml
    GTest::gtest
    benchmark::benchmark)

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

include(FetchContent)

FetchContent_Declare(
  googletest SOURCE_DIR ${OPENTELEMETRY_CPP_SRC_DIR}/third_party/googletest)

FetchContent_MakeAvailable(googletest)

message(STATUS "opentelemetry-cpp_SRC_DIR: ${OPENTELEMETRY_CPP_SRC_DIR}")

set(BUILD_TESTING ON)

add_executable(api_only_test
               ${OPENTELEMETRY_CPP_SRC_DIR}/install/test/src/test_api.cc)

target_link_libraries(api_only_test PRIVATE opentelemetry-cpp::api GTest::gtest
                                            GTest::gtest_main)

include(CTest)
include(GoogleTest)

gtest_add_tests(
  TARGET api_only_test
  TEST_PREFIX api_only.
  TEST_LIST api_only_test)
