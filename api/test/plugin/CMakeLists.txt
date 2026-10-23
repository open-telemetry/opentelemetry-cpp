# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

add_executable(dynamic_load_test dynamic_load_test.cc)
target_link_libraries(dynamic_load_test ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
target_link_libraries(dynamic_load_test ${CMAKE_DL_LIBS})
gtest_discover_tests(dynamic_load_test TEST_PREFIX plugin.)
