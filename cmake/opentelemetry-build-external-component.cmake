# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

function(get_directory_name_in_path PATH_VAR RESULT_VAR)
  # get_filename_component does not work with paths ending in / or \, so remove it.
  string(REGEX REPLACE "[/\\]$" "" PATH_TRIMMED "${PATH_VAR}")

  get_filename_component(DIR_NAME ${PATH_TRIMMED} NAME)

  set(${RESULT_VAR} "${DIR_NAME}" PARENT_SCOPE)
endfunction()

# Enable building external components through otel-cpp build
# The config options are
#  - OPENTELEMETRY_EXTERNAL_COMPONENT_PATH - Setting local paths of the external
# component as env variable. Multiple paths can be set by separating them with.

# Add custom vendor component from local path
# Prefer CMake option, then env variable, then URL.
if(OPENTELEMETRY_EXTERNAL_COMPONENT_PATH)
  # Add custom component path to build tree and consolidate binary artifacts in
  # current project binary output directory.
  foreach(DIR IN LISTS OPENTELEMETRY_EXTERNAL_COMPONENT_PATH)
    get_directory_name_in_path(${DIR} EXTERNAL_EXPORTER_DIR_NAME)
    add_subdirectory(${DIR} ${PROJECT_BINARY_DIR}/external/${EXTERNAL_EXPORTER_DIR_NAME})
  endforeach()
endif()
