# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

add_library(component_a STATIC component_a.cc)
add_library(component_b STATIC component_b.cc)
add_library(component_c SHARED component_c.cc)
add_library(component_d SHARED component_d.cc)

add_executable(singleton_test singleton_test.cc)

target_link_libraries(singleton_test
   component_a component_b component_c component_d
   ${GTEST_BOTH_LIBRARIES} opentelemetry_api)

gtest_add_tests(
  TARGET singleton_test
  TEST_PREFIX singleton.
  TEST_LIST singleton_test)

