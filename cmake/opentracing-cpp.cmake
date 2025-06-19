# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

otel_add_thirdparty_package(
  PACKAGE_NAME "OpenTracing"
  PACKAGE_SEARCH_MODES "CONFIG"
  FETCH_NAME "opentracing"
  FETCH_GIT_REPOSITORY "https://github.com/opentracing/opentracing-cpp.git"
  FETCH_GIT_TAG "${opentracing-cpp_GIT_TAG}"
  FETCH_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/opentracing-cpp"
  FETCH_CMAKE_ARGS
    "-DBUILD_TESTING=OFF"
  VERSION_REGEX "#define[ \t]+OPENTRACING_VERSION[ \t]+\"([0-9]+(\\.[0-9]+)*)\""
  VERSION_FILE "\${opentracing_BINARY_DIR}/include/opentracing/version.h"
)

function(_patch_opentracing_targets)
  get_property(_OpenTracing_SOURCE_DIR DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_OpenTracing_SOURCE_DIR)
  get_property(_OpenTracing_BINARY_DIR DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_OpenTracing_BINARY_DIR)
  foreach(_target opentracing opentracing-static)
    if(TARGET ${_target})
      # Add missing include directories
      target_include_directories(${_target} PUBLIC
        "$<BUILD_INTERFACE:${_OpenTracing_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${_OpenTracing_SOURCE_DIR}/3rd_party/include>"
        "$<BUILD_INTERFACE:${_OpenTracing_BINARY_DIR}/include>"
        )
      # Disable CXX_INCLUDE_WHAT_YOU_USE and CXX_CLANG_TIDY
      set_target_properties(${_target}
                          PROPERTIES CXX_INCLUDE_WHAT_YOU_USE "" CXX_CLANG_TIDY "")
      # Create alias targets
      if(NOT TARGET OpenTracing::${_target})
        add_library(OpenTracing::${_target} ALIAS ${_target})
      endif()
    endif()
  endforeach()
endfunction()

_patch_opentracing_targets()

if(NOT TARGET OpenTracing::opentracing AND NOT TARGET OpenTracing::opentracing-static)
  message(FATAL_ERROR "No OpenTracing targets (OpenTracing::opentracing or OpenTracing::opentracing-static) were imported")
endif()
