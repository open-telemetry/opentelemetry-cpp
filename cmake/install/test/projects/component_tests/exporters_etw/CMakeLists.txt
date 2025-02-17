# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)
project(opentelemetry-cpp-exporters_etw-install-test LANGUAGES CXX)

find_package(opentelemetry-cpp REQUIRED COMPONENTS exporters_etw)

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(exporters_etw_test ${INSTALL_TEST_SRC_DIR}/test_exporters_etw.cc)
target_link_libraries(exporters_etw_test PRIVATE opentelemetry-cpp::etw_exporter
                                                 GTest::GTest GTest::Main)

gtest_discover_tests(exporters_etw_test)
