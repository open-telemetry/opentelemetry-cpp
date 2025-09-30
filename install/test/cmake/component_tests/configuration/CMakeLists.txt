# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)
project(opentelemetry-cpp-configuration-install-test LANGUAGES CXX)

find_package(opentelemetry-cpp REQUIRED COMPONENTS sdk)

if(NOT TARGET Threads::Threads)
  message(FATAL_ERROR "Threads::Threads target not found")
endif()

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(configuration_test ${INSTALL_TEST_SRC_DIR}/test_configuration.cc)
target_link_libraries(
  configuration_test
  PRIVATE opentelemetry-cpp::configuration
          GTest::gtest
          GTest::gtest_main)

gtest_discover_tests(configuration_test)
