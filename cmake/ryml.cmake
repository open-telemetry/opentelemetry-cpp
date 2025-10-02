# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Import the rapidyaml target (ryml::ryml).
# 1. Find an installed ryml package
# 2. Use FetchContent to fetch and build ryml from GitHub

find_package(ryml QUIET)
set(ryml_PROVIDER "find_package")

if(NOT ryml_FOUND)
  FetchContent_Declare(
    "ryml"
    GIT_REPOSITORY "https://github.com/biojppm/rapidyaml.git"
    GIT_TAG "${ryml_GIT_TAG}"
    )
  set(ryml_PROVIDER "fetch_repository")

  set(RYML_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
  set(RYML_BUILD_API OFF CACHE BOOL "" FORCE)
  set(RYML_DBG OFF CACHE BOOL "" FORCE)
  set(RYML_INSTALL ${OPENTELEMETRY_INSTALL} CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(ryml)

  # Set the ryml_VERSION variable from the git tag.
  string(REGEX REPLACE "^v([0-9]+\\.[0-9]+\\.[0-9]+)$" "\\1" ryml_VERSION "${ryml_GIT_TAG}")
endif()

if(NOT TARGET ryml::ryml)
  message(FATAL_ERROR "The required ryml::ryml target was not imported")
endif()
