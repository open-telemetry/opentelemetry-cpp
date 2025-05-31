# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(load)

if(NOT OPENTELEMETRY_SKIP_DYNAMIC_LOADING_TESTS)
  add_subdirectory(plugin)
endif()

if(BUILD_TESTING)
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/load/empty_config.txt" "")
  add_test(
    NAME examples.plugin
    COMMAND
      "$<TARGET_FILE:load_plugin_example>" "$<TARGET_FILE:example_plugin>"
      "${CMAKE_CURRENT_BINARY_DIR}/load/empty_config.txt")
endif()
