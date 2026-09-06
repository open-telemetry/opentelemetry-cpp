# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)
project(opentelemetry-cpp-pkgconfig-install-test LANGUAGES CXX)

if(NOT DEFINED INSTALL_TEST_SRC_DIR)
  message(
    FATAL_ERROR
      "INSTALL_TEST_SRC_DIR must be defined when running cmake on this test project"
  )
endif()

find_package(PkgConfig REQUIRED)
find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

# ---------------------------------------------------------------------------
# API consumer (interface library: empty Libs, Cflags from compile definitions)
# ---------------------------------------------------------------------------
pkg_check_modules(OTEL_API REQUIRED opentelemetry_api)

add_executable(pkgconfig_api_test ${INSTALL_TEST_SRC_DIR}/test_api.cc)
target_include_directories(pkgconfig_api_test PRIVATE ${OTEL_API_INCLUDE_DIRS})
target_compile_options(pkgconfig_api_test PRIVATE ${OTEL_API_CFLAGS_OTHER})
target_link_libraries(pkgconfig_api_test PRIVATE ${OTEL_API_LIBRARIES}
                                                  ${OTEL_API_LDFLAGS} GTest::gtest
                                                  GTest::gtest_main)

gtest_discover_tests(pkgconfig_api_test)

# ---------------------------------------------------------------------------
# OStream per-module Requires chain validation.
# Each executable uses exactly one pkg_check_modules() call so a broken Requires
# in one .pc file cannot be masked by unrelated modules.
# ---------------------------------------------------------------------------
function(add_ostream_pkgconfig_chain_test test_name pc_module)
  pkg_check_modules(${test_name}_PC REQUIRED ${pc_module})

  set(_source "${CMAKE_CURRENT_BINARY_DIR}/${test_name}.cc")
  file(WRITE "${_source}" "int main() { return 0; }\n")

  add_executable(${test_name} "${_source}")
  target_include_directories(${test_name} PRIVATE ${${test_name}_PC_INCLUDE_DIRS})
  target_compile_options(${test_name} PRIVATE ${${test_name}_PC_CFLAGS_OTHER})
  target_link_libraries(${test_name} PRIVATE ${${test_name}_PC_LIBRARIES}
                                             ${${test_name}_PC_LDFLAGS})

  add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

add_ostream_pkgconfig_chain_test(
  ostream_pc_chain_span opentelemetry_exporter_ostream_span)
add_ostream_pkgconfig_chain_test(
  ostream_pc_chain_span_builder opentelemetry_exporter_ostream_span_builder)
add_ostream_pkgconfig_chain_test(
  ostream_pc_chain_metrics opentelemetry_exporter_ostream_metrics)
add_ostream_pkgconfig_chain_test(
  ostream_pc_chain_metrics_builder opentelemetry_exporter_ostream_metrics_builder)
add_ostream_pkgconfig_chain_test(
  ostream_pc_chain_logs opentelemetry_exporter_ostream_logs)
add_ostream_pkgconfig_chain_test(
  ostream_pc_chain_logs_builder opentelemetry_exporter_ostream_logs_builder)

# ---------------------------------------------------------------------------
# Full OStream install source test. Resolve each OStream module separately, then
# combine flags/libraries for the shared integration source.
# ---------------------------------------------------------------------------
pkg_check_modules(OTEL_OSTREAM_SPAN REQUIRED opentelemetry_exporter_ostream_span)
pkg_check_modules(OTEL_OSTREAM_SPAN_BUILDER REQUIRED
                  opentelemetry_exporter_ostream_span_builder)
pkg_check_modules(OTEL_OSTREAM_METRICS REQUIRED opentelemetry_exporter_ostream_metrics)
pkg_check_modules(OTEL_OSTREAM_METRICS_BUILDER REQUIRED
                  opentelemetry_exporter_ostream_metrics_builder)
pkg_check_modules(OTEL_OSTREAM_LOGS REQUIRED opentelemetry_exporter_ostream_logs)
pkg_check_modules(OTEL_OSTREAM_LOGS_BUILDER REQUIRED
                  opentelemetry_exporter_ostream_logs_builder)

# Builder sources in test_exporters_ostream.cc include configuration headers.
pkg_check_modules(OTEL_CONFIGURATION_CORE REQUIRED opentelemetry_configuration_core)

add_executable(pkgconfig_ostream_test ${INSTALL_TEST_SRC_DIR}/test_exporters_ostream.cc)

set(_ostream_include_dirs
    ${OTEL_OSTREAM_SPAN_INCLUDE_DIRS} ${OTEL_OSTREAM_SPAN_BUILDER_INCLUDE_DIRS}
    ${OTEL_OSTREAM_METRICS_INCLUDE_DIRS}
    ${OTEL_OSTREAM_METRICS_BUILDER_INCLUDE_DIRS}
    ${OTEL_OSTREAM_LOGS_INCLUDE_DIRS} ${OTEL_OSTREAM_LOGS_BUILDER_INCLUDE_DIRS}
    ${OTEL_CONFIGURATION_CORE_INCLUDE_DIRS})
list(REMOVE_DUPLICATES _ostream_include_dirs)
target_include_directories(pkgconfig_ostream_test PRIVATE ${_ostream_include_dirs})

set(_ostream_cflags
    ${OTEL_OSTREAM_SPAN_CFLAGS_OTHER} ${OTEL_OSTREAM_SPAN_BUILDER_CFLAGS_OTHER}
    ${OTEL_OSTREAM_METRICS_CFLAGS_OTHER}
    ${OTEL_OSTREAM_METRICS_BUILDER_CFLAGS_OTHER}
    ${OTEL_OSTREAM_LOGS_CFLAGS_OTHER} ${OTEL_OSTREAM_LOGS_BUILDER_CFLAGS_OTHER}
    ${OTEL_CONFIGURATION_CORE_CFLAGS_OTHER})
target_compile_options(pkgconfig_ostream_test PRIVATE ${_ostream_cflags})

target_link_libraries(
  pkgconfig_ostream_test
  PRIVATE ${OTEL_OSTREAM_SPAN_LIBRARIES}
          ${OTEL_OSTREAM_SPAN_LDFLAGS}
          ${OTEL_OSTREAM_SPAN_BUILDER_LIBRARIES}
          ${OTEL_OSTREAM_SPAN_BUILDER_LDFLAGS}
          ${OTEL_OSTREAM_METRICS_LIBRARIES}
          ${OTEL_OSTREAM_METRICS_LDFLAGS}
          ${OTEL_OSTREAM_METRICS_BUILDER_LIBRARIES}
          ${OTEL_OSTREAM_METRICS_BUILDER_LDFLAGS}
          ${OTEL_OSTREAM_LOGS_LIBRARIES}
          ${OTEL_OSTREAM_LOGS_LDFLAGS}
          ${OTEL_OSTREAM_LOGS_BUILDER_LIBRARIES}
          ${OTEL_OSTREAM_LOGS_BUILDER_LDFLAGS}
          ${OTEL_CONFIGURATION_CORE_LIBRARIES}
          ${OTEL_CONFIGURATION_CORE_LDFLAGS}
          GTest::gtest
          GTest::gtest_main)

gtest_discover_tests(pkgconfig_ostream_test)
