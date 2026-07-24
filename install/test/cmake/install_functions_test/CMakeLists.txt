# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# opentelemetry-cpp CMake Install Functions Tests
#
# These tests cover the CMake install functions defined in
# install/cmake/install_functions.cmake.
#
# The tests require mock components to be added to the opentelemetry-cpp CMake
# project using OPENTELEMETRY_EXTERNAL_COMPONENT_PATH set to import componenents
# from install/test/cmake/install_functions_test/components

# Usage: ./ci/do_ci.sh cmake.install_functions.test

cmake_minimum_required(VERSION 3.16)
project(opentelemetry-cpp-install-functions-tests LANGUAGES NONE)

enable_testing()

# ---------------------------------------------------------------
# Test find_package with no components specified. All installed components and
# targets must be imported.
# ----------------------------------------------------------------
add_test(
  NAME install-functions-usage-no-components-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/post_install_tests/no_components -B
    build-cmake-usage-no-components-test
    "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}" ${INSTALL_TEST_CMAKE_OPTIONS})

# ---------------------------------------------------------------
# Test find_package with a deprecated component name. A deprecation warning must
# be emitted and the replacement component must be imported.
# ---------------------------------------------------------------
add_test(
  NAME install-functions-deprecated-name-test
  COMMAND
    ${CMAKE_COMMAND} -Wdeprecated --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/post_install_tests/deprecated_name -B
    build-deprecated-name-test "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

set_tests_properties(
  install-functions-deprecated-name-test
  PROPERTIES PASS_REGULAR_EXPRESSION "CMake Deprecation Warning"
             FAIL_REGULAR_EXPRESSION "CMake Error")

# ---------------------------------------------------------------
# Test find_package component dependency resolution. Unsorted components must be
# imported in the correct order.
# ---------------------------------------------------------------
add_test(
  NAME install-functions-unsorted-components-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/post_install_tests/unsorted_components -B
    build-unsorted-components-test "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

# ---------------------------------------------------------------
# Test find_package component dependency resolution. Missing dependent
# components must be imported automatically.
# ---------------------------------------------------------------
add_test(
  NAME install-functions-missing-components-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/post_install_tests/missing_components -B
    build-missing-components-test "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

# ---------------------------------------------------------------
# Test find_package on an unsupported/unknown component
# ---------------------------------------------------------------
add_test(
  NAME install-functions-unsupported-components-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/post_install_tests/unsupported_components -B
    build-unsupported-components-test
    "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")
