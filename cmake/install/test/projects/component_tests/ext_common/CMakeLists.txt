# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)
project(opentelemetry-cpp-ext_common-install-test LANGUAGES CXX)

find_package(opentelemetry-cpp REQUIRED COMPONENTS ext_common)

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(ext_common_test ${INSTALL_TEST_SRC_DIR}/test_ext_common.cc)
target_link_libraries(ext_common_test PRIVATE opentelemetry-cpp::ext
                                              GTest::GTest GTest::Main)

gtest_discover_tests(ext_common_test)
