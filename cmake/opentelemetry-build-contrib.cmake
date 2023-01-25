# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Enable building external components and/or cotrib repo through otel-cpp build
# The config options are
#  - OPENTELEMETRY_CONTRIB_PATH - Setting local path of the external
# component as env variable
#  - Enable WITH_CONTRIB CMake option to build otel-cpp-contrib repo as part of
# otel-cpp build.
# Add custom vendor directory via environment variable

option(WITH_CONTRIB "Build a tarball package" OFF)

if(DEFINED ENV{OPENTELEMETRY_CONTRIB_PATH})
  # Add custom contrib path to build tree and consolidate binary artifacts in
  # current project binary output directory.
  add_subdirectory($ENV{OPENTELEMETRY_CONTRIB_PATH}
                   ${PROJECT_BINARY_DIR}/contrib)
endif()
# OR Add opentelemetry-cpp-contrib from GitHub
if(WITH_CONTRIB)
  # This option requires CMake 3.11+: add standard remote contrib to build tree.
  include(FetchContent)
  FetchContent_Declare(
    contrib
    GIT_REPOSITORY "https://github.com/open-telemetry/opentelemetry-cpp-contrib"
    GIT_TAG "main")
  FetchContent_GetProperties(contrib)
  if(NOT contrib_POPULATED)
    FetchContent_Populate(contrib)
    add_subdirectory(${contrib_SOURCE_DIR})
  endif()
endif()
