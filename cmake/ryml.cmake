# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Import the rapidyaml target (ryml::ryml).
# 1. Find an installed ryml package
# 2. Use FetchContent to build ryml from a git submodule
# 3. Use FetchContent to fetch and build ryml from GitHub

# Set the FIND_ryml_VERSION variable from the git tag.
string(REGEX REPLACE "^v([0-9]+\\.[0-9]+\\.[0-9]+)$" "\\1" FIND_ryml_VERSION "${ryml_GIT_TAG}")

# TODO: use OTEL_ryml_SEARCH_MODE ?
find_package(ryml ${FIND_ryml_VERSION} CONFIG QUIET)
set(ryml_PROVIDER "find_package")

if(NOT ryml_FOUND)
  set(_RYML_SUBMODULE_DIR "${opentelemetry-cpp_SOURCE_DIR}/third_party/ryml")
  if(EXISTS "${_RYML_SUBMODULE_DIR}/.git")
    FetchContent_Declare(
      "ryml"
      SOURCE_DIR "${_RYML_SUBMODULE_DIR}"
      )
    set(ryml_PROVIDER "fetch_source")
  else()
    FetchContent_Declare(
      "ryml"
      GIT_REPOSITORY "https://github.com/biojppm/rapidyaml.git"
      GIT_TAG "${ryml_GIT_TAG}"
      )
    set(ryml_PROVIDER "fetch_repository")
  endif()

  set(RYML_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  set(RYML_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(ryml)

  # Set the ryml_VERSION variable from the git tag.
  string(REGEX REPLACE "^v([0-9]+\\.[0-9]+\\.[0-9]+)$" "\\1" ryml_VERSION "${ryml_GIT_TAG}")
endif()

if(NOT TARGET ryml::ryml)
  message(FATAL_ERROR "The required ryml::ryml target was not imported")
endif()
