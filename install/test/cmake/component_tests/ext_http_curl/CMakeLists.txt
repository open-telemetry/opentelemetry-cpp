# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)
project(opentelemetry-cpp-ext_http_curl-install-test LANGUAGES CXX)

find_package(opentelemetry-cpp REQUIRED COMPONENTS ext_http_curl)

if(NOT TARGET CURL::libcurl)
  message(FATAL_ERROR "CURL::libcurl target not found")
endif()

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(ext_http_curl_test ${INSTALL_TEST_SRC_DIR}/test_ext_http_curl.cc)
target_link_libraries(
  ext_http_curl_test PRIVATE opentelemetry-cpp::http_client_curl GTest::gtest
                             GTest::gtest_main)

gtest_discover_tests(ext_http_curl_test)

include(${CMAKE_CURRENT_LIST_DIR}/../../otel_header_probes.cmake)

otel_add_header_probes(
  TARGET
  ext_http_curl_header_probes
  LINK
  opentelemetry-cpp::http_client_curl
  HEADERS
  opentelemetry/ext/http/client/curl/http_client_curl.h
  opentelemetry/ext/http/client/curl/http_client_factory_curl.h
  opentelemetry/ext/http/client/curl/http_operation_curl.h)
