# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set(_SAVED_BUILD_TESTING ${BUILD_TESTING})

otel_add_thirdparty_package(
  PACKAGE_NAME "OpenTracing"
  PACKAGE_SEARCH_MODES "CONFIG"
  FETCH_NAME "opentracing"
  FETCH_GIT_REPOSITORY "https://github.com/opentracing/opentracing-cpp.git"
  FETCH_GIT_TAG "${opentracing-cpp_GIT_TAG}"
  FETCH_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/opentracing-cpp"
  FETCH_CMAKE_ARGS
    BUILD_TESTING=OFF
  VERSION_REGEX "#define[ \t]+OPENTRACING_VERSION[ \t]+\"([0-9]+(\\.[0-9]+)*)\""
  VERSION_FILE "\${opentracing_BINARY_DIR}/include/opentracing/version.h"
)

set(BUILD_TESTING ${_SAVED_BUILD_TESTING} CACHE BOOL "" FORCE)
unset(_SAVED_BUILD_TESTING)

if(NOT ${OpenTracing_PROVIDER} STREQUAL "package")
  if(TARGET opentracing AND NOT TARGET OpenTracing::opentracing)
    target_include_directories(opentracing PUBLIC
      "$<BUILD_INTERFACE:${opentracing_SOURCE_DIR}/include>"
      "$<BUILD_INTERFACE:${opentracing_SOURCE_DIR}/3rd_party/include>"
      "$<BUILD_INTERFACE:${opentracing_BINARY_DIR}/include>"
      )
    set_target_properties(opentracing
                        PROPERTIES CXX_INCLUDE_WHAT_YOU_USE "" CXX_CLANG_TIDY "")
    add_library(OpenTracing::opentracing ALIAS opentracing)
  endif()

  if(TARGET opentracing-static AND NOT TARGET OpenTracing::opentracing-static)
    target_include_directories(opentracing-static PUBLIC
      "$<BUILD_INTERFACE:${opentracing_SOURCE_DIR}/include>"
      "$<BUILD_INTERFACE:${opentracing_SOURCE_DIR}/3rd_party/include>"
      "$<BUILD_INTERFACE:${opentracing_BINARY_DIR}/include>"
      )
    set_target_properties(opentracing-static PROPERTIES
                        POSITION_INDEPENDENT_CODE ON
                        CXX_INCLUDE_WHAT_YOU_USE "" CXX_CLANG_TIDY "")

    add_library(OpenTracing::opentracing-static ALIAS opentracing-static)
  endif()
endif()

if(NOT TARGET OpenTracing::opentracing AND NOT TARGET OpenTracing::opentracing-static)
  message(FATAL_ERROR "A required OpenTracing target (opentracing or opentracing-static) was not found")
endif()
