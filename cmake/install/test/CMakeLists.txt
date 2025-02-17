# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)
project(opentelemetry-cpp-component-install-tests LANGUAGES CXX)

if(CXX_STANDARD)
  message(STATUS "CMAKE_CXX_STANDARD is set from CXX_STANDARD=${CXX_STANDARD}")
  set(CMAKE_CXX_STANDARD $CXX_STANDARD)
else()
  set(CMAKE_CXX_STANDARD 14)
  message(
    STATUS "CMAKE_CXX_STANDARD is set to default of ${CMAKE_CXX_STANDARD}")
endif()

include(${CMAKE_SOURCE_DIR}/../../install/component-definitions.cmake)

# If COMPONENTS_TO_TEST is not set, then test all components
if(NOT COMPONENTS_TO_TEST)
  set(COMPONENTS_TO_TEST ${opentelemetry-cpp_COMPONENTS})
else()
  set(COMPONENTS_ARE_VALID TRUE)
  foreach(component ${COMPONENTS_TO_TEST})
    if(NOT component IN_LIST opentelemetry-cpp_COMPONENTS)
      message(
        ERROR
        " Component ${component} is not a valid opentelemetry-cpp component")
      set(COMPONENTS_ARE_VALID FALSE)
    endif()
  endforeach()
  if(NOT COMPONENTS_ARE_VALID)
    message(FATAL_ERROR "COMPONENTS_TO_TEST contains invalid components")
  endif()
endif()

set(INSTALL_TEST_SRC_DIR "${CMAKE_SOURCE_DIR}/src")

enable_testing()

message(STATUS "Testing cmake usage for find_package(opentelemetry ...)")
# -----------------------------------------------------------
# CMake Usage tests for find_package(opentelemetry-cpp ...)
#
# 1. Test find_package with no components specified
# 2. Test find_package with components specified but not sorted in dependency
#    order
# 3. Test find_package with components specified but missing dependent components
# 4. Test find_package with components specified but including
#    unsupported/unknown components

add_test(
  NAME cmake-usage-no-components-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/projects/cmake_usage_tests/no_components -B
    build-cmake-usage-no-components-test
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH})

add_test(
  NAME cmake-usage-unsorted-components-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/projects/cmake_usage_tests/unsorted_components -B
    build-cmake-usage-unsorted-components-test
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH})

add_test(
  NAME cmake-usage-missing-components-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/projects/cmake_usage_tests/missing_components -B
    build-cmake-usage-missing-components-test
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH})

add_test(
  NAME cmake-usage-unsupported-components-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/projects/cmake_usage_tests/unsupported_components -B
    build-cmake-usage-unsupported-components-test
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH})

# -----------------------------------------------------------
# Test the full package install using legacy cmake build instructions
# find_package(opentelemetry-cpp CONFIG REQUIRED)
message(STATUS "Testing the full package install")
# Test cmake configuration
add_test(
  NAME full-package-cmake-config-test
  COMMAND
    ${CMAKE_COMMAND} --log-level=DEBUG -S
    ${CMAKE_SOURCE_DIR}/projects/package_test -B build-full-package-test
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    -DINSTALL_TEST_SRC_DIR=${INSTALL_TEST_SRC_DIR})

# Build the full package test executable
add_test(NAME full-package-build-test
         COMMAND ${CMAKE_COMMAND} --build
                 ${CMAKE_BINARY_DIR}/build-full-package-test --parallel)

# Run the full package test executable
add_test(NAME full-package-run-test
         COMMAND ${CMAKE_BINARY_DIR}/build-full-package-test/full_test)
# -----------------------------------------------------------

# -----------------------------------------------------------
# Loop over all the components to test
foreach(component ${COMPONENTS_TO_TEST})
  message(STATUS "Testing the ${component} component install")
  # Test cmake configuration
  add_test(
    NAME component-${component}-cmake-config-test
    COMMAND
      ${CMAKE_COMMAND} --log-level=DEBUG -S
      ${CMAKE_SOURCE_DIR}/projects/component_tests/${component} -B
      build-${component}-test -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
      -DINSTALL_TEST_SRC_DIR=${INSTALL_TEST_SRC_DIR})

  # Build the component test executable
  add_test(NAME component-${component}-build-test
           COMMAND ${CMAKE_COMMAND} --build
                   ${CMAKE_BINARY_DIR}/build-${component}-test --parallel)

  # Run the component test executable
  add_test(
    NAME component-${component}-run-test
    COMMAND ${CMAKE_BINARY_DIR}/build-${component}-test/${component}_test)
endforeach()
