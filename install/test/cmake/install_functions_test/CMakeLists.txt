# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# opentelemetry-cpp CMake Install Functions Tests
#
# These tests cover the opentelemetry-cpp CMake install functions defined in
# cmake/otel-install-functions.cmake.
#
# Usage: ./ci/do_ci.sh cmake.install_functions.test

cmake_minimum_required(VERSION 3.16)
project(opentelemetry-cpp-install-functions-tests LANGUAGES NONE)

list(
  APPEND
  INSTALL_TEST_CMAKE_OPTIONS
  "-DCMAKE_CXX_STANDARD=17"
  "-DCMAKE_BUILD_TYPE=Release"
  "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
  "-DENABLE_EXAMPLES=OFF"
  "-DENABLE_TESTS=OFF"
  "-DENABLE_PREVIEW=OFF"
  "-DENABLE_COMPONENTS=OFF"
  "-DOPENTELEMETRY_INSTALL=ON"
  "-DCMAKE_INSTALL_PREFIX=${INSTALL_TEST_DIR}"
  "-C"
  "${OPENTELEMETRY_SOURCE_DIR}/test_common/cmake/all-options-abiv1.cmake"
  "-S"
  "${OPENTELEMETRY_SOURCE_DIR}")

set(POST_INSTALL_TEST_CMAKE_OPTIONS "-DCMAKE_PREFIX_PATH=${INSTALL_TEST_DIR}")

enable_testing()

# ---------------------------------------------------------------
# Test to verify that a currently defined component cannot be marked deprecated
# by another component. Must fail the CMake configure step.
# ---------------------------------------------------------------
add_test(
  NAME install-functions-configure-duplicate-component-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -B
    build-configure-duplicate-component-test
    "-DOPENTELEMETRY_EXTERNAL_COMPONENT_PATH=${CMAKE_SOURCE_DIR}/configure_tests/duplicate_component"
    ${INSTALL_TEST_CMAKE_OPTIONS})
set_tests_properties(install-functions-configure-duplicate-component-test
                     PROPERTIES WILL_FAIL TRUE)

# ---------------------------------------------------------------
# Test to verify that duplicate component names cannot be defined. Must fail the
# CMake configure step.
# ---------------------------------------------------------------
add_test(
  NAME install-functions-configure-component-name-collision-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -B
    build-configure-component-name-collision-test
    "-DOPENTELEMETRY_EXTERNAL_COMPONENT_PATH=${CMAKE_SOURCE_DIR}/configure_tests/component_name_collision"
    ${INSTALL_TEST_CMAKE_OPTIONS})
set_tests_properties(install-functions-configure-component-name-collision-test
                     PROPERTIES WILL_FAIL TRUE)

# ---------------------------------------------------------------
# Test to verify that a deprecated component cannot have multiple replacement
# components. Must fail the CMake configure step.
# ---------------------------------------------------------------
add_test(
  NAME install-functions-configure-duplicate-deprecated-name-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -B
    build-configure-duplicate-deprecated-name-test
    "-DOPENTELEMETRY_EXTERNAL_COMPONENT_PATH=${CMAKE_SOURCE_DIR}/configure_tests/duplicate_deprecated_name"
    ${INSTALL_TEST_CMAKE_OPTIONS})
set_tests_properties(install-functions-configure-duplicate-deprecated-name-test
                     PROPERTIES WILL_FAIL TRUE)

# ---------------------------------------------------------------
# Test to correctly define CMake components including component to component
# dependencies and deprecated components. Must pass the CMake configure step and
# installation must succeed for the tests to follow.
# ---------------------------------------------------------------
add_test(
  NAME install-functions-configure-valid-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -B build-configure-valid-test
    "-DOPENTELEMETRY_EXTERNAL_COMPONENT_PATH=${CMAKE_SOURCE_DIR}/configure_tests/valid_components"
    ${INSTALL_TEST_CMAKE_OPTIONS})

add_test(NAME install-functions-install-valid-test
         COMMAND ${CMAKE_COMMAND} --build build-configure-valid-test --parallel
                 --target install)
set_tests_properties(install-functions-install-valid-test
                     PROPERTIES DEPENDS install-functions-configure-valid-test)

# ---------------------------------------------------------------
# Test find_package with no components specified. All installed components and
# targets must be imported.
# ----------------------------------------------------------------
add_test(
  NAME install-functions-usage-no-components-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/post_install_tests/no_components -B
    build-cmake-usage-no-components-test ${POST_INSTALL_TEST_CMAKE_OPTIONS})
set_tests_properties(install-functions-usage-no-components-test
                     PROPERTIES DEPENDS install-functions-install-valid-test)

# ---------------------------------------------------------------
# Test find_package with a deprecated component name. A deprecation warning must
# be emitted and the replacement component must be imported.
# ---------------------------------------------------------------
add_test(
  NAME install-functions-deprecated-name-test
  COMMAND
    ${CMAKE_COMMAND} -Wdeprecated --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/post_install_tests/deprecated_name -B
    build-deprecated-name-test ${POST_INSTALL_TEST_CMAKE_OPTIONS})
set_tests_properties(install-functions-deprecated-name-test
                     PROPERTIES DEPENDS install-functions-install-valid-test)

# ---------------------------------------------------------------
# Test find_package component dependency resolution. Unsorted components must be
# imported in the correct order.
# ---------------------------------------------------------------
add_test(
  NAME install-functions-unsorted-components-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/post_install_tests/unsorted_components -B
    build-unsorted-components-test ${POST_INSTALL_TEST_CMAKE_OPTIONS})
set_tests_properties(install-functions-unsorted-components-test
                     PROPERTIES DEPENDS install-functions-install-valid-test)

# ---------------------------------------------------------------
# Test find_package component dependency resolution. Missing dependent
# components must be imported automatically.
# ---------------------------------------------------------------
add_test(
  NAME install-functions-missing-components-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/post_install_tests/missing_components -B
    build-missing-components-test ${POST_INSTALL_TEST_CMAKE_OPTIONS})
set_tests_properties(install-functions-missing-components-test
                     PROPERTIES DEPENDS install-functions-install-valid-test)

# ---------------------------------------------------------------
# Test find_package on an unsupported/unknown component
# ---------------------------------------------------------------
add_test(
  NAME install-functions-unsupported-components-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/post_install_tests/unsupported_components -B
    build-unsupported-components-test ${POST_INSTALL_TEST_CMAKE_OPTIONS})
set_tests_properties(install-functions-unsupported-components-test
                     PROPERTIES DEPENDS install-functions-install-valid-test)
