# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Enable building external components through otel-cpp build
# The config options are
#  - OPENTELEMETRY_EXTERNAL_COMPONENT_PATH - Setting local path of the external
# component as env variable
#  - OPENTELEMETRY_EXTERNAL_COMPONENT_URL Setting githb-repo of external component
#  as env variable


# Add custom vendor component from local path, or GitHub repo
if(DEFINED $ENV{OPENTELEMETRY_EXTERNAL_COMPONENT_PATH})
  # Add custom component path to build tree and consolidate binary artifacts in
  # current project binary output directory.
  add_subdirectory($ENV{OPENTELEMETRY_EXTERNAL_COMPONENT_PATH}
                   ${PROJECT_BINARY_DIR}/external)
elseif(DEFINED $ENV{OPENTELEMETRY_EXTERNAL_COMPONENT_URL})
  # This option requires CMake 3.11+: add standard remote repo to build tree.
  include(FetchContent)
  FetchContent_Declare(
    external
    GIT_REPOSITORY $ENV{OPENTELEMETRY_EXTERNAL_COMPONENT_URL}
    GIT_TAG "main")
  FetchContent_GetProperties(external)
  if(NOT external_POPULATED)
    FetchContent_Populate(external)
    add_subdirectory(${external_SOURCE_DIR})
  endif()
endif()
