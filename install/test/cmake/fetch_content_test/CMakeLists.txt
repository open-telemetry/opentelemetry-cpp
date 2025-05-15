# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# This test uses CMake's FetchContent module to build opentelemetry-cpp from src
# and make its targets available within an external project.

cmake_minimum_required(VERSION 3.14)

project(opentelemetry-cpp-fetch-content-test LANGUAGES CXX)

find_package(GTest CONFIG REQUIRED)

set(BUILD_TESTING
    ON
    CACHE BOOL "Build tests" FORCE)

if(NOT DEFINED OPENTELEMETRY_CPP_SRC_DIR)
  message(
    FATAL_ERROR
      "OPENTELEMETRY_CPP_SRC_DIR must be defined when running cmake on this test project"
  )
endif()

message(
  STATUS
    "Adding opentelemetry-cpp as a subdirectory with FetchContent from ${OPENTELEMETRY_CPP_SRC_DIR}"
)

include(FetchContent)
FetchContent_Declare(opentelemetry-cpp SOURCE_DIR ${OPENTELEMETRY_CPP_SRC_DIR})
FetchContent_MakeAvailable(opentelemetry-cpp)

add_executable(
  fetch_content_src_test
  ${OPENTELEMETRY_CPP_SRC_DIR}/install/test/src/test_api.cc
  ${OPENTELEMETRY_CPP_SRC_DIR}/install/test/src/test_sdk.cc)

target_link_libraries(
  fetch_content_src_test
  PRIVATE opentelemetry-cpp::api
          opentelemetry-cpp::version
          opentelemetry-cpp::metrics
          opentelemetry-cpp::trace
          opentelemetry-cpp::logs
          GTest::gtest
          GTest::gtest_main)

include(CTest)
include(GoogleTest)

gtest_add_tests(
  TARGET fetch_content_src_test
  TEST_PREFIX fetch_content.
  TEST_LIST fetch_content_src_test)
