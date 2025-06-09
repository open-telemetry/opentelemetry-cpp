# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.14)

# See https://cmake.org/cmake/help/latest/policy/CMP0074.html required by
# certain version of zlib which CURL depends on.
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.12")
  cmake_policy(SET CMP0074 NEW)
endif()

# Allow to use normal variable for option()
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13")
  cmake_policy(SET CMP0077 NEW)
endif()

# Prefer CMAKE_MSVC_RUNTIME_LIBRARY if possible
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

if(POLICY CMP0092)
  # https://cmake.org/cmake/help/latest/policy/CMP0092.html#policy:CMP0092 Make
  # sure the /W3 is not removed from CMAKE_CXX_FLAGS since CMake 3.15
  cmake_policy(SET CMP0092 OLD)
endif()

# MSVC RTTI flag /GR should not be not added to CMAKE_CXX_FLAGS by default. @see
# https://cmake.org/cmake/help/latest/policy/CMP0117.html
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.20.0")
  cmake_policy(SET CMP0117 NEW)
endif()

# Read the opentelemetry-cpp project and abi version from the api version header
# file and set the third-party version tags
include("${CMAKE_CURRENT_LIST_DIR}/cmake/version.cmake")

project(
  opentelemetry-cpp
  VERSION ${OPENTELEMETRY_VERSION}
  LANGUAGES CXX)

# Mark variables as used so cmake doesn't complain about them
mark_as_advanced(CMAKE_TOOLCHAIN_FILE)

# Autodetect vcpkg toolchain
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

option(WITH_ABI_VERSION_1 "ABI version 1" ON)
option(WITH_ABI_VERSION_2 "EXPERIMENTAL: ABI version 2 preview" OFF)

#
# We do not want to have WITH_ABI_VERSION = "1" or "2", and instead prefer two
# distinct flags, WITH_ABI_VERSION_1 and WITH_ABI_VERSION_2.
#
# This allows:
#
# * to have a specific option description for each ABI
# * to mark experimental/stable/deprecated on flags, for clarity
# * to search for exact abi usage move easily, discouraging:
#
#   * cmake -DWITH_ABI_VERSION=${ARG}
#
# While not supported, having distinct WITH_ABI_VERSION_1 and WITH_ABI_VERSION_2
# flags also opens the possibility to support multiple ABI concurrently, should
# that become necessary.
#
if(WITH_ABI_VERSION_1 AND WITH_ABI_VERSION_2)
  #
  # Only one ABI is supported in a build.
  #
  message(
    FATAL_ERROR "Set either WITH_ABI_VERSION_1 or WITH_ABI_VERSION_2, not both")
endif()

if(WITH_ABI_VERSION_2)
  set(OPENTELEMETRY_ABI_VERSION_NO "2")
elseif(WITH_ABI_VERSION_1)
  set(OPENTELEMETRY_ABI_VERSION_NO "1")
else()
  set(OPENTELEMETRY_ABI_VERSION_NO ${OPENTELEMETRY_ABI_VERSION_DEFAULT})
endif()

option(WITH_NO_DEPRECATED_CODE "Do not include deprecated code" OFF)

set(WITH_STL
    "OFF"
    CACHE STRING "Which version of the Standard Library for C++ to use")

option(WITH_GSL
       "Whether to use Guidelines Support Library for C++ latest features" OFF)

set(OPENTELEMETRY_INSTALL_default ON)
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(OPENTELEMETRY_INSTALL_default OFF)
endif()
option(OPENTELEMETRY_INSTALL "Whether to install opentelemetry targets"
       ${OPENTELEMETRY_INSTALL_default})

include("${opentelemetry-cpp_SOURCE_DIR}/cmake/tools.cmake")
include("${opentelemetry-cpp_SOURCE_DIR}/cmake/otel-install-functions.cmake")

if(NOT WITH_STL STREQUAL "OFF")
  # These definitions are needed for test projects that do not link against
  # opentelemetry-api library directly. We ensure that variant implementation
  # (absl::variant or std::variant) in variant unit test code is consistent with
  # the global project build definitions. Optimize for speed to reduce the hops
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(CMAKE_BUILD_TYPE MATCHES Debug)
      # Turn off optimizations for DEBUG
      set(MSVC_CXX_OPT_FLAG "/Od")
    else()
      string(REGEX MATCH "\/O" result ${CMAKE_CXX_FLAGS})
      if(NOT ${result} MATCHES "\/O")
        set(MSVC_CXX_OPT_FLAG "/O2")
      endif()
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_CXX_OPT_FLAG}")
  endif()
endif()

option(WITH_OTLP_RETRY_PREVIEW
       "Whether to enable experimental retry functionality" OFF)

option(WITH_OTLP_GRPC_SSL_MTLS_PREVIEW
       "Whether to enable mTLS support for gRPC" OFF)

option(WITH_OTLP_GRPC_CREDENTIAL_PREVIEW
       "Whether to enable gRPC credentials option in OTLP gRPC Exporter" OFF)

option(WITH_OTLP_GRPC "Whether to include the OTLP gRPC exporter in the SDK"
       OFF)

option(WITH_OTLP_HTTP "Whether to include the OTLP http exporter in the SDK"
       OFF)

option(WITH_OTLP_FILE "Whether to include the OTLP file exporter in the SDK"
       OFF)

option(
  WITH_OTLP_HTTP_COMPRESSION
  "Whether to include gzip compression for the OTLP http exporter in the SDK"
  OFF)

option(WITH_CURL_LOGGING "Whether to enable select CURL verbosity in OTel logs"
       OFF)

option(WITH_ZIPKIN "Whether to include the Zipkin exporter in the SDK" OFF)

option(WITH_PROMETHEUS "Whether to include the Prometheus Client in the SDK"
       OFF)

option(WITH_ELASTICSEARCH
       "Whether to include the Elasticsearch Client in the SDK" OFF)

option(WITH_NO_GETENV "Whether the platform supports environment variables" OFF)

option(BUILD_TESTING "Whether to enable tests" ON)

option(WITH_BENCHMARK "Whether to build benchmark program" ON)

option(BUILD_W3CTRACECONTEXT_TEST "Whether to build w3c trace context" OFF)

option(OTELCPP_MAINTAINER_MODE "Build in maintainer mode (-Wall -Werror)" OFF)

option(WITH_OPENTRACING "Whether to include the Opentracing shim" OFF)

option(OTELCPP_VERSIONED_LIBS "Whether to generate the versioned shared libs"
       OFF)

#
# This option is experimental, subject to change in the spec:
#
# * https://github.com/open-telemetry/opentelemetry-specification/issues/2232
#
option(WITH_REMOVE_METER_PREVIEW
       "EXPERIMENTAL, ABI BREAKING: Allow to remove a meter" OFF)

if(OTELCPP_VERSIONED_LIBS AND NOT BUILD_SHARED_LIBS)
  message(FATAL_ERROR "OTELCPP_VERSIONED_LIBS=ON requires BUILD_SHARED_LIBS=ON")
endif()

if(WIN32)
  option(WITH_ETW "Whether to include the ETW Exporter in the SDK" ON)
else()
  if(DEFINED (WITH_ETW))
    message(FATAL_ERROR "WITH_ETW is only supported on Windows")
  endif()
endif(WIN32)

# Do not convert deprecated message to error
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
  add_compile_options(-Wno-error=deprecated-declarations)
endif()

option(
  WITH_API_ONLY
  "Only build the API (use as a header-only library). Overrides WITH_EXAMPLES and all options to enable exporters"
  OFF)
option(WITH_EXAMPLES "Whether to build examples" ON)

# This requires CURL, OFF by default.
option(
  WITH_EXAMPLES_HTTP
  "Whether to build http client/server examples. Requires WITH_EXAMPLES and CURL"
  OFF)

option(WITH_FUNC_TESTS "Whether to build functional tests" ON)

option(WITH_ASYNC_EXPORT_PREVIEW "Whether to enable async export" OFF)

# Exemplar specs status is experimental, so behind feature flag by default
option(WITH_METRICS_EXEMPLAR_PREVIEW
       "Whether to enable exemplar within metrics" OFF)

# Experimental, so behind feature flag by default
option(WITH_THREAD_INSTRUMENTATION_PREVIEW
       "Whether to enable thread instrumentation" OFF)

option(OPENTELEMETRY_SKIP_DYNAMIC_LOADING_TESTS
       "Whether to build test libraries that are always linked as shared libs"
       OFF)

option(
  OTELCPP_THIRDPARTY_FETCH_CONTENT_ONLY
  "Whether to require use of FetchContent to satisfy dependencies from submodules or GitHub repositories"
  OFF)

option(
  OTELCPP_THIRDPARTY_FIND_PACKAGE_ONLY
  "Whether to require use of find_package to satisfy dependencies with locally installed packages"
  OFF)

#
# Verify options dependencies
#
if(OTELCPP_THIRDPARTY_FETCH_CONTENT_ONLY
   AND OTELCPP_THIRDPARTY_FIND_PACKAGE_ONLY)
  message(
    FATAL_ERROR
      "OTELCPP_THIRDPARTY_FETCH_CONTENT_ONLY and "
      "OTELCPP_THIRDPARTY_FIND_PACKAGE_ONLY are mutually "
      "exclusive options. Both can be OFF but only one can be ON.")
endif()
if(WITH_EXAMPLES_HTTP AND NOT WITH_EXAMPLES)
  message(FATAL_ERROR "WITH_EXAMPLES_HTTP=ON requires WITH_EXAMPLES=ON")
endif()

if(MSVC)
  # Options for Visual C++ compiler: /Zc:__cplusplus - report an updated value
  # for recent C++ language standards. Without this option MSVC returns the
  # value of __cplusplus="199711L"
  if(MSVC_VERSION GREATER 1900)
    # __cplusplus flag is not supported by Visual Studio 2015
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
  endif()
endif()

#
# API ONLY Dependencies
#

if(WITH_GSL)
  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/ms-gsl.cmake")
endif()

#
# SDK and Exporter Dependencies
#

if(NOT WITH_API_ONLY)

  if(WITH_OPENTRACING)
    include("${opentelemetry-cpp_SOURCE_DIR}/cmake/opentracing-cpp.cmake")
  endif()

  # Always include Threads for the SDK

  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/threads.cmake")

  #
  # Do we need HTTP CLIENT CURL ?
  #

  set(WITH_HTTP_CLIENT_CURL OFF)
  if(WITH_OTLP_HTTP
     OR WITH_ELASTICSEARCH
     OR WITH_ZIPKIN
     OR BUILD_W3CTRACECONTEXT_TEST
     OR WITH_EXAMPLES_HTTP)
    set(WITH_HTTP_CLIENT_CURL ON)
  endif()

  #
  # Do we need ZLIB ?
  #

  if(WITH_HTTP_CLIENT_CURL AND WITH_OTLP_HTTP_COMPRESSION)
    include("${opentelemetry-cpp_SOURCE_DIR}/cmake/zlib.cmake")
  endif()

  #
  # Do we need CURL ?
  #

  if(WITH_HTTP_CLIENT_CURL)
    include("${opentelemetry-cpp_SOURCE_DIR}/cmake/curl.cmake")
  endif()

  #
  # Do we need gRPC, Protobuf, and opentelemetry-proto ?
  #

  if(WITH_OTLP_GRPC
     OR WITH_OTLP_HTTP
     OR WITH_OTLP_FILE)

    # Include gRPC before Protobuf We'll use the gRPC submodules for Protobuf
    # and Abseil with FetchContent

    if(WITH_OTLP_GRPC)
      include("${opentelemetry-cpp_SOURCE_DIR}/cmake/grpc.cmake")
    endif()

    include("${opentelemetry-cpp_SOURCE_DIR}/cmake/protobuf.cmake")
    include("${opentelemetry-cpp_SOURCE_DIR}/cmake/opentelemetry-proto.cmake")
  endif()

  #
  # Do we need NLOHMANN_JSON ?
  #

  if(WITH_ELASTICSEARCH
     OR WITH_ZIPKIN
     OR WITH_OTLP_HTTP
     OR WITH_OTLP_FILE
     OR BUILD_W3CTRACECONTEXT_TEST
     OR WITH_ETW)
    include("${opentelemetry-cpp_SOURCE_DIR}/cmake/nlohmann-json.cmake")
  endif()

  if(WITH_PROMETHEUS)
    include("${opentelemetry-cpp_SOURCE_DIR}/cmake/prometheus-cpp.cmake")
  endif()

endif(NOT WITH_API_ONLY)

if(OTELCPP_MAINTAINER_MODE)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Building with gcc in maintainer mode.")

    add_compile_options(-Wall)
    add_compile_options(-Werror)
    add_compile_options(-Wextra)

    # Tested with GCC 9.4 on github.
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 9.4)
      message(STATUS "Building with additional warnings for gcc.")

      # Relaxed warnings

      # Enforced warnings

      # C++ options only
      add_compile_options($<$<STREQUAL:$<COMPILE_LANGUAGE>,CXX>:-Wextra-semi>)
      add_compile_options(
        $<$<STREQUAL:$<COMPILE_LANGUAGE>,CXX>:-Woverloaded-virtual>)
      add_compile_options(
        $<$<STREQUAL:$<COMPILE_LANGUAGE>,CXX>:-Wsuggest-override>)
      add_compile_options(
        $<$<STREQUAL:$<COMPILE_LANGUAGE>,CXX>:-Wold-style-cast>)

      # C and C++
      add_compile_options(-Wcast-qual)
      add_compile_options(-Wformat-security)
      add_compile_options(-Wlogical-op)
      add_compile_options(-Wmissing-include-dirs)
      add_compile_options(-Wstringop-truncation)
      add_compile_options(-Wundef)
      add_compile_options(-Wvla)
    endif()
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Building with clang in maintainer mode.")

    add_compile_options(-Wall)
    add_compile_options(-Werror)
    add_compile_options(-Wextra)

    # Tested with Clang 11.0 on github.
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 11.0)
      message(STATUS "Building with additional warnings for clang.")

      # Relaxed warnings
      add_compile_options(-Wno-error=unused-private-field)

      # Enforced warnings
      add_compile_options(-Wcast-qual)
      add_compile_options(-Wconditional-uninitialized)
      add_compile_options(-Wextra-semi)
      add_compile_options(-Wformat-security)
      add_compile_options(-Wheader-hygiene)
      add_compile_options(-Winconsistent-missing-destructor-override)
      add_compile_options(-Winconsistent-missing-override)
      add_compile_options(-Wnewline-eof)
      add_compile_options(-Wnon-virtual-dtor)
      add_compile_options(-Woverloaded-virtual)
      add_compile_options(-Wrange-loop-analysis)
      add_compile_options(-Wundef)
      add_compile_options(-Wundefined-reinterpret-cast)
      add_compile_options(-Wvla)
      add_compile_options(-Wold-style-cast)
    endif()
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Building with msvc in maintainer mode.")

    add_compile_options(/WX)
    add_compile_options(/W4)

    # Relaxed warnings
    add_compile_options(/wd4100)
    add_compile_options(/wd4125)
    add_compile_options(/wd4566)
    add_compile_options(/wd4127)
    add_compile_options(/wd4512)
    add_compile_options(/wd4267)
    add_compile_options(/wd4996)

    # Enforced warnings
    add_compile_options(/we4265) # 'class': class has virtual functions, but
                                 # destructor is not virtual
    add_compile_options(/we5204) # A class with virtual functions has
                                 # non-virtual trivial destructor.

  elseif()
    message(FATAL_ERROR "Building with unknown compiler in maintainer mode.")
  endif()
endif(OTELCPP_MAINTAINER_MODE)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

if(BUILD_TESTING)
  include(CTest)
  include(GoogleTest)
  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/googletest.cmake")
  enable_testing()

  if(WITH_BENCHMARK)
    include("${opentelemetry-cpp_SOURCE_DIR}/cmake/benchmark.cmake")
  endif()
endif()

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  if(NOT WIN32)
    message(FATAL_ERROR "Build DLL is only supported on Windows!")
  endif()
  if(NOT MSVC)
    message(WARNING "Build DLL is supposed to work with MSVC!")
  endif()
  if(WITH_STL)
    message(
      WARNING "Build DLL with C++ STL could cause binary incompatibility!")
  endif()
  add_definitions(-DOPENTELEMETRY_BUILD_EXPORT_DLL)
endif()

otel_print_build_config()

add_subdirectory(api)

if(NOT WITH_API_ONLY)

  if(WITH_OPENTRACING)
    add_subdirectory(opentracing-shim)
  endif()

  add_subdirectory(sdk)
  add_subdirectory(ext)
  add_subdirectory(exporters)

  if(BUILD_TESTING)
    add_subdirectory(test_common)
  endif()
  if(WITH_EXAMPLES)
    add_subdirectory(examples)
  endif()
  if(WITH_FUNC_TESTS)
    add_subdirectory(functional)
  endif()
endif()

include(
  "${opentelemetry-cpp_SOURCE_DIR}/cmake/opentelemetry-build-external-component.cmake"
)
include("${opentelemetry-cpp_SOURCE_DIR}/cmake/patch-imported-config.cmake")

if(OPENTELEMETRY_INSTALL)
  # Install the cmake config and version files
  otel_install_cmake_config()

  # Install the components and associated files
  otel_install_components()

  # Install the thirdparty dependency definition file
  otel_install_thirdparty_definitions()

  if(BUILD_PACKAGE)
    include("${opentelemetry-cpp_SOURCE_DIR}/cmake/package.cmake")
    include(CPack)
  endif()
endif()

# Make the version and ABI version available to the user for in-tree builds
# These are the same variables that are cached with
# find_package(opentelemetry-cpp CONFIG)
set(OPENTELEMETRY_ABI_VERSION_NO
    "${OPENTELEMETRY_ABI_VERSION_NO}"
    CACHE STRING "opentelemetry-cpp ABI version" FORCE)
set(OPENTELEMETRY_VERSION
    "${OPENTELEMETRY_VERSION}"
    CACHE STRING "opentelemetry-cpp version" FORCE)
