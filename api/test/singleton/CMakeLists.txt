# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

add_library(component_a STATIC component_a.cc)

add_library(component_b STATIC component_b.cc)

add_library(component_c SHARED component_c.cc)
set_target_properties(component_c PROPERTIES CXX_VISIBILITY_PRESET default)

add_library(component_d SHARED component_d.cc)
set_target_properties(component_d PROPERTIES CXX_VISIBILITY_PRESET hidden)

add_library(component_e SHARED component_e.cc)
set_target_properties(component_e PROPERTIES CXX_VISIBILITY_PRESET default)

add_library(component_f SHARED component_f.cc)
set_target_properties(component_f PROPERTIES CXX_VISIBILITY_PRESET hidden)

add_executable(singleton_test singleton_test.cc)

target_link_libraries(
  singleton_test
  component_a
  component_b
  component_c
  component_d
  component_e
  component_f
  ${GTEST_BOTH_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  opentelemetry_api)

if(NOT WIN32)

  gtest_add_tests(
    TARGET singleton_test
    TEST_PREFIX singleton.
    TEST_LIST singleton_test)

endif()
