# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)

# MSVC RTTI flag /GR should not be added to CMAKE_CXX_FLAGS by default. @see
# https://cmake.org/cmake/help/latest/policy/CMP0117.html
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.20.0")
  cmake_policy(SET CMP0117 NEW)
endif()

set(OPENTELEMETRY_VERSION_NUMBER "1.29.0")
set(OPENTELEMETRY_VERSION_SUFFIX "-dev")
set(OPENTELEMETRY_VERSION
    "${OPENTELEMETRY_VERSION_NUMBER}${OPENTELEMETRY_VERSION_SUFFIX}")

set(OPENTELEMETRY_ABI_VERSION_DEFAULT "1")

project(
  opentelemetry-cpp
  VERSION "${OPENTELEMETRY_VERSION_NUMBER}"
  LANGUAGES CXX)

# Mark variables as used so cmake doesn't complain about them
mark_as_advanced(CMAKE_TOOLCHAIN_FILE)

include("${opentelemetry-cpp_SOURCE_DIR}/cmake/tools.cmake")

# Set the third-party release git tags.
if(EXISTS "${opentelemetry-cpp_SOURCE_DIR}/third_party_release")
  file(STRINGS "${opentelemetry-cpp_SOURCE_DIR}/third_party_release"
       third_party_tags)
  foreach(_raw_line IN LISTS third_party_tags)
    # Strip leading/trailing whitespace
    string(STRIP "${_raw_line}" _line)
    # Skip empty lines and comments
    if(_line STREQUAL "" OR _line MATCHES "^#")
      continue()
    endif()

    # Match "package_name=git_tag"
    if(_line MATCHES "^([^=]+)=(.+)$")
      set(_third_party_name "${CMAKE_MATCH_1}")
      set(_git_tag "${CMAKE_MATCH_2}")
      set("${_third_party_name}_GIT_TAG" "${_git_tag}")
    else()
      message(
        FATAL_ERROR
          "Could not parse third-party tag. Invalid line in ${opentelemetry-cpp_SOURCE_DIR}/third_party_release. Line:\n  ${_raw_line}"
      )
    endif()
  endforeach()
endif()

# Autodetect vcpkg toolchain
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

otelcpp_option_flag(OTELCPP_WITH_ABI_VERSION_1 "ABI version 1" ON)
otelcpp_option_flag(OTELCPP_WITH_ABI_VERSION_2
                    "EXPERIMENTAL: ABI version 2 preview" OFF)

set(_OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW_REQUESTED OFF)
if(DEFINED OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW)
  set(_OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW_REQUESTED
      "${OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW}")
elseif(DEFINED WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW)
  set(_OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW_REQUESTED
      "${WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW}")
endif()
if(_OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW_REQUESTED
   AND NOT OTELCPP_WITH_ABI_VERSION_2)
  message(
    FATAL_ERROR
      "OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW requires OTELCPP_WITH_ABI_VERSION_2"
  )
endif()
unset(_OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW_REQUESTED)

# Experimental: bound synchronous metric instruments (Counter, Histogram).
# Requires OTELCPP_WITH_ABI_VERSION_2. Context-bearing bound operations,
# exemplar parity, Gauge, and UpDownCounter bound support are follow-ups.
otelcpp_dependent_option(
  OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW
  "EXPERIMENTAL: bound synchronous metric instruments preview" OFF
  "OTELCPP_WITH_ABI_VERSION_2" OFF)

otelcpp_option_flag(OTELCPP_WITH_CONFIGURATION
                    "EXPERIMENTAL: YAML configuration file" OFF)

#
# We do not want to have OTELCPP_WITH_ABI_VERSION = "1" or "2", and instead
# prefer two distinct flags, OTELCPP_WITH_ABI_VERSION_1 and
# OTELCPP_WITH_ABI_VERSION_2.
#
# This allows:
#
# * to have a specific option description for each ABI
# * to mark experimental/stable/deprecated on flags, for clarity
# * to search for exact abi usage move easily, discouraging:
#
#   * cmake -DOTELCPP_WITH_ABI_VERSION=${ARG}
#
# While not supported, having distinct OTELCPP_WITH_ABI_VERSION_1 and
# OTELCPP_WITH_ABI_VERSION_2 flags also opens the possibility to support
# multiple ABI concurrently, should that become necessary.
#
if(OTELCPP_WITH_ABI_VERSION_1 AND OTELCPP_WITH_ABI_VERSION_2)
  #
  # Only one ABI is supported in a build.
  #
  message(
    FATAL_ERROR
      "Set either OTELCPP_WITH_ABI_VERSION_1 or OTELCPP_WITH_ABI_VERSION_2, not both"
  )
endif()

if(OTELCPP_WITH_ABI_VERSION_2)
  set(OPENTELEMETRY_ABI_VERSION_NO "2")
elseif(OTELCPP_WITH_ABI_VERSION_1)
  set(OPENTELEMETRY_ABI_VERSION_NO "1")
else()
  set(OPENTELEMETRY_ABI_VERSION_NO "${OPENTELEMETRY_ABI_VERSION_DEFAULT}")
endif()

if(OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW
   AND NOT OTELCPP_WITH_ABI_VERSION_2)
  message(
    FATAL_ERROR
      "OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW requires OTELCPP_WITH_ABI_VERSION_2"
  )
endif()

otelcpp_option_flag(OTELCPP_WITH_NO_DEPRECATED_CODE
                    "Do not include deprecated code" OFF)

otelcpp_option_variable(OTELCPP_WITH_STL "OFF" STRING
                        "Which version of the Standard Library for C++ to use")

otelcpp_option_flag(
  OTELCPP_WITH_GSL
  "Whether to use Guidelines Support Library for C++ latest features" OFF)

set(OTELCPP_INSTALL_DEFAULT ON)
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(OTELCPP_INSTALL_DEFAULT OFF)
endif()
otelcpp_option_flag(OTELCPP_INSTALL "Whether to install opentelemetry targets"
                    ${OTELCPP_INSTALL_DEFAULT})

if(NOT OTELCPP_WITH_STL STREQUAL "OFF")
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

otelcpp_option_flag(OTELCPP_WITH_OTLP_RETRY_PREVIEW
                    "Whether to enable retry functionality" ON)

otelcpp_option_flag(OTELCPP_WITH_OTLP_GRPC_SSL_MTLS_PREVIEW
                    "Whether to enable mTLS support for gRPC" ON)

otelcpp_option_flag(
  OTELCPP_WITH_OTLP_GRPC_CREDENTIAL_PREVIEW
  "Whether to enable gRPC credentials option in OTLP gRPC Exporter" OFF)

otelcpp_option_flag(OTELCPP_WITH_OTLP_GRPC
                    "Whether to include the OTLP gRPC exporter in the SDK" OFF)

otelcpp_option_flag(OTELCPP_WITH_OTLP_HTTP
                    "Whether to include the OTLP http exporter in the SDK" OFF)

otelcpp_option_flag(OTELCPP_WITH_OTLP_FILE
                    "Whether to include the OTLP file exporter in the SDK" OFF)

otelcpp_option_flag(
  OTELCPP_WITH_OTLP_HTTP_COMPRESSION
  "Whether to include gzip compression for the OTLP http exporter in the SDK"
  OFF)

otelcpp_option_flag(
  OTELCPP_WITH_OTLP_UTF8_VALIDITY
  "Whether to enable UTF-8 validity checks for the OTLP exporter in the SDK" ON)

otelcpp_option_flag(OTELCPP_WITH_CURL_LOGGING
                    "Whether to enable select CURL verbosity in OTel logs" OFF)

otelcpp_option_flag(OTELCPP_WITH_ZIPKIN
                    "Whether to include the Zipkin exporter in the SDK" OFF)

otelcpp_option_flag(OTELCPP_WITH_PROMETHEUS
                    "Whether to include the Prometheus Client in the SDK" OFF)

otelcpp_option_flag(
  OTELCPP_WITH_ELASTICSEARCH
  "Whether to include the Elasticsearch Client in the SDK" OFF)

otelcpp_option_flag(OTELCPP_WITH_NO_GETENV
                    "Whether the platform supports environment variables" OFF)

# BUILD_TESTING is a standard CMake variable, so use it only as the top-level
# default and do not treat it as a deprecated alias.
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  if(DEFINED BUILD_TESTING)
    option(OTELCPP_BUILD_TESTING "Whether to enable tests" "${BUILD_TESTING}")
  else()
    option(OTELCPP_BUILD_TESTING "Whether to enable tests" ON)
  endif()
  otelcpp_option_flag(
    OTELCPP_WITH_BENCHMARK "Whether to build benchmark program"
    ${OTELCPP_BUILD_TESTING})
else()
  option(OTELCPP_BUILD_TESTING "Whether to enable tests" OFF)
  otelcpp_option_flag(OTELCPP_WITH_BENCHMARK
                      "Whether to build benchmark program" OFF)
endif()

otelcpp_option_flag(OTELCPP_BUILD_W3CTRACECONTEXT_TEST
                    "Whether to build w3c trace context" OFF)

otelcpp_option_flag(OTELCPP_MAINTAINER_MODE
                    "Build in maintainer mode (-Wall -Werror)" OFF)

otelcpp_option_flag(OTELCPP_WITH_OPENTRACING
                    "Whether to include the Opentracing shim" OFF)

otelcpp_option_flag(OTELCPP_VERSIONED_LIBS
                    "Whether to generate the versioned shared libs" OFF)
otelcpp_option_flag(
  OTELCPP_BUILD_DLL
  "Whether to build the OpenTelemetry C++ SDK as a Windows DLL" OFF)

otelcpp_option_flag(OTELCPP_BUILD_PACKAGE "Whether to generate a package" OFF)

otelcpp_option_variable(
  OTELCPP_EXTERNAL_COMPONENT_PATH "" STRING
  "Semicolon-separated paths to external components to build")

if(OTELCPP_VERSIONED_LIBS AND NOT BUILD_SHARED_LIBS)
  message(FATAL_ERROR "OTELCPP_VERSIONED_LIBS=ON requires BUILD_SHARED_LIBS=ON")
endif()

if(WIN32)
  otelcpp_option_flag(OTELCPP_WITH_ETW
                      "Whether to include the ETW Exporter in the SDK" ON)
else()
  if(DEFINED OTELCPP_WITH_ETW OR DEFINED WITH_ETW)
    message(FATAL_ERROR "OTELCPP_WITH_ETW is only supported on Windows")
  endif()
endif(WIN32)

# Do not convert deprecated message to error
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
  add_compile_options(-Wno-error=deprecated-declarations)
endif()

otelcpp_option_flag(
  OTELCPP_WITH_API_ONLY
  "Only build the API (use as a header-only library). Overrides OTELCPP_WITH_EXAMPLES and all options to enable exporters"
  OFF)
otelcpp_option_flag(OTELCPP_WITH_EXAMPLES "Whether to build examples" ON)

# This requires CURL, OFF by default.
otelcpp_option_flag(
  OTELCPP_WITH_EXAMPLES_HTTP
  "Whether to build http client/server examples. Requires OTELCPP_WITH_EXAMPLES and CURL"
  OFF)

otelcpp_option_flag(OTELCPP_WITH_FUNC_TESTS "Whether to build functional tests"
                    ON)

otelcpp_option_flag(OTELCPP_WITH_ASYNC_EXPORT_PREVIEW
                    "Whether to enable async export" OFF)

# Exemplar specs status is experimental, so behind feature flag by default
otelcpp_option_flag(OTELCPP_WITH_METRICS_EXEMPLAR_PREVIEW
                    "Whether to enable exemplar within metrics" OFF)

# Experimental, so behind feature flag by default
otelcpp_option_flag(OTELCPP_WITH_THREAD_INSTRUMENTATION_PREVIEW
                    "Whether to enable thread instrumentation" OFF)

otelcpp_option_flag(OTELCPP_WITH_RESOURCE_DETECTORS_PREVIEW
                    "Whether to enable inbuilt resource detectors" OFF)

otelcpp_option_flag(
  OTELCPP_SKIP_DYNAMIC_LOADING_TESTS
  "Whether to build test libraries that are always linked as shared libs" OFF)

#
# Verify options dependencies
#

include(FetchContent)

if(OTELCPP_WITH_EXAMPLES_HTTP AND NOT OTELCPP_WITH_EXAMPLES)
  message(
    FATAL_ERROR
      "OTELCPP_WITH_EXAMPLES_HTTP=ON requires OTELCPP_WITH_EXAMPLES=ON")
endif()

if(OTELCPP_WITH_GSL)
  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/ms-gsl.cmake")
endif()

if(NOT OTELCPP_WITH_API_ONLY)
  find_package(Threads)
endif()

function(set_target_version target_name)
  if(OTELCPP_VERSIONED_LIBS)
    set_target_properties(
      ${target_name} PROPERTIES VERSION ${OPENTELEMETRY_VERSION}
                                SOVERSION ${OPENTELEMETRY_ABI_VERSION_NO})
  endif()
endfunction()

if(MSVC)
  # Options for Visual C++ compiler: /Zc:__cplusplus - report an updated value
  # for recent C++ language standards. Without this option MSVC returns the
  # value of __cplusplus="199711L"
  if(MSVC_VERSION GREATER 1900)
    # __cplusplus flag is not supported by Visual Studio 2015
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
  endif()
endif()

# include GNUInstallDirs before include cmake/opentelemetry-proto.cmake because
# opentelemetry-proto installs targets to the location variables defined in
# GNUInstallDirs.
include(GNUInstallDirs)

#
# Do we need HTTP CLIENT CURL ?
#

otelcpp_dependent_option(
  OTELCPP_WITH_HTTP_CLIENT_CURL
  "Use the curl HTTP client backend. Defaults to ON when any HTTP exporter is enabled; set OFF to supply a custom transport."
  ON
  "OTELCPP_WITH_OTLP_HTTP OR OTELCPP_WITH_ELASTICSEARCH OR OTELCPP_WITH_ZIPKIN OR OTELCPP_BUILD_W3CTRACECONTEXT_TEST OR OTELCPP_WITH_EXAMPLES_HTTP"
  OFF)

#
# Locally override options if OTELCPP_WITH_API_ONLY is set to prevent third
# party dependencies from being imported
#

if(OTELCPP_WITH_API_ONLY)
  set(OTELCPP_WITH_EXAMPLES OFF)
  set(OTELCPP_WITH_EXAMPLES_HTTP OFF)
  set(OTELCPP_WITH_OTLP_GRPC OFF)
  set(OTELCPP_WITH_OTLP_HTTP OFF)
  set(OTELCPP_WITH_OTLP_FILE OFF)
  set(OTELCPP_WITH_ZIPKIN OFF)
  set(OTELCPP_WITH_PROMETHEUS OFF)
  set(OTELCPP_WITH_ELASTICSEARCH OFF)
  set(OTELCPP_WITH_OPENTRACING OFF)
  set(OTELCPP_WITH_ETW OFF)
  set(OTELCPP_WITH_CONFIGURATION OFF)
  set(OTELCPP_WITH_HTTP_CLIENT_CURL OFF)
  set(OTELCPP_WITH_OTLP_HTTP_COMPRESSION OFF)
  set(OTELCPP_WITH_BENCHMARK OFF)
  set(OTELCPP_WITH_FUNC_TESTS OFF)
  set(OTELCPP_BUILD_TESTING OFF)
  set(OTELCPP_BUILD_W3CTRACECONTEXT_TEST OFF)
endif()

#
# Do we need ZLIB ?
#

if(OTELCPP_WITH_HTTP_CLIENT_CURL AND OTELCPP_WITH_OTLP_HTTP_COMPRESSION)
  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/zlib.cmake")
endif()

#
# Do we need CURL ?
#

if(OTELCPP_WITH_HTTP_CLIENT_CURL)
  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/curl.cmake")
endif()

#
# Do we need prometheus-cpp ?
#

if(OTELCPP_WITH_PROMETHEUS)
  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/prometheus-cpp.cmake")
endif()

#
# Do we need protobuf and/or gRPC ?
#

if(OTELCPP_WITH_OTLP_GRPC
   OR OTELCPP_WITH_OTLP_HTTP
   OR OTELCPP_WITH_OTLP_FILE)

  # find or fetch grpc before protobuf to allow protobuf to be built in-tree as
  # a grpc submodule.
  if(OTELCPP_WITH_OTLP_GRPC)
    include("${opentelemetry-cpp_SOURCE_DIR}/cmake/grpc.cmake")
  endif()

  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/protobuf.cmake")

  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/opentelemetry-proto.cmake")
endif()

#
# Do we need NLOHMANN_JSON ?
#

if(OTELCPP_WITH_ELASTICSEARCH
   OR OTELCPP_WITH_ZIPKIN
   OR OTELCPP_WITH_OTLP_HTTP
   OR OTELCPP_WITH_OTLP_FILE
   OR OTELCPP_BUILD_W3CTRACECONTEXT_TEST
   OR OTELCPP_WITH_ETW)
  set(USE_NLOHMANN_JSON ON)
else()
  set(USE_NLOHMANN_JSON OFF)
endif()

if(USE_NLOHMANN_JSON)
  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/nlohmann-json.cmake")
endif()

#
# Do we need RapidYaml ?
#

if(OTELCPP_WITH_CONFIGURATION)
  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/ryml.cmake")
endif()

#
# Do we need OpenTracing ?
#

if(OTELCPP_WITH_OPENTRACING)
  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/opentracing-cpp.cmake")
endif()

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  # Keep CTest's standard option aligned for top-level builds without exposing
  # it as an opentelemetry-cpp subproject setting.
  set(BUILD_TESTING "${OTELCPP_BUILD_TESTING}")
  include(CTest)
elseif(OTELCPP_BUILD_TESTING)
  enable_testing()
endif()

if(OTELCPP_BUILD_TESTING)
  include("${opentelemetry-cpp_SOURCE_DIR}/cmake/googletest.cmake")
  if(OTELCPP_WITH_BENCHMARK)
    include("${opentelemetry-cpp_SOURCE_DIR}/cmake/benchmark.cmake")
  endif()
endif()

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

# Record build config and versions
message(STATUS "---------------------------------------------")
message(STATUS "build settings")
message(STATUS "---------------------------------------------")
message(STATUS "OpenTelemetry VERSION: ${OPENTELEMETRY_VERSION}")
message(STATUS "OpenTelemetry ABI: ${OPENTELEMETRY_ABI_VERSION_NO}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CXX: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CXXFLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")

message(STATUS "---------------------------------------------")
message(STATUS "opentelemetry-cpp build options")
message(STATUS "---------------------------------------------")
message(STATUS "OTELCPP_WITH_API_ONLY: ${OTELCPP_WITH_API_ONLY}")
message(
  STATUS "OTELCPP_WITH_NO_DEPRECATED_CODE: ${OTELCPP_WITH_NO_DEPRECATED_CODE}")
message(STATUS "OTELCPP_WITH_ABI_VERSION_1: ${OTELCPP_WITH_ABI_VERSION_1}")
message(STATUS "OTELCPP_WITH_ABI_VERSION_2: ${OTELCPP_WITH_ABI_VERSION_2}")
message(STATUS "OTELCPP_VERSIONED_LIBS: ${OTELCPP_VERSIONED_LIBS}")
message(STATUS "OTELCPP_BUILD_PACKAGE: ${OTELCPP_BUILD_PACKAGE}")
message(
  STATUS "OTELCPP_EXTERNAL_COMPONENT_PATH: ${OTELCPP_EXTERNAL_COMPONENT_PATH}")
message(STATUS "OTELCPP_MAINTAINER_MODE: ${OTELCPP_MAINTAINER_MODE}")
message(STATUS "OTELCPP_WITH_STL: ${OTELCPP_WITH_STL}")
message(STATUS "OTELCPP_WITH_GSL: ${OTELCPP_WITH_GSL}")
message(STATUS "OTELCPP_WITH_NO_GETENV: ${OTELCPP_WITH_NO_GETENV}")
message(STATUS "OTELCPP_INSTALL: ${OTELCPP_INSTALL}")

message(STATUS "---------------------------------------------")
message(STATUS "opentelemetry-cpp cmake component options")
message(STATUS "---------------------------------------------")
message(STATUS "OTELCPP_WITH_OTLP_GRPC: ${OTELCPP_WITH_OTLP_GRPC}")
message(STATUS "OTELCPP_WITH_OTLP_HTTP: ${OTELCPP_WITH_OTLP_HTTP}")
message(STATUS "OTELCPP_WITH_OTLP_FILE: ${OTELCPP_WITH_OTLP_FILE}")
message(
  STATUS "OTELCPP_WITH_HTTP_CLIENT_CURL: ${OTELCPP_WITH_HTTP_CLIENT_CURL}")
message(STATUS "OTELCPP_WITH_ZIPKIN: ${OTELCPP_WITH_ZIPKIN}")
message(STATUS "OTELCPP_WITH_PROMETHEUS: ${OTELCPP_WITH_PROMETHEUS}")
message(STATUS "OTELCPP_WITH_ELASTICSEARCH: ${OTELCPP_WITH_ELASTICSEARCH}")
message(STATUS "OTELCPP_WITH_OPENTRACING: ${OTELCPP_WITH_OPENTRACING}")
message(STATUS "OTELCPP_WITH_ETW: ${OTELCPP_WITH_ETW}")
message(STATUS "OTELCPP_BUILD_DLL: ${OTELCPP_BUILD_DLL}")
message(STATUS "OTELCPP_WITH_CONFIGURATION: ${OTELCPP_WITH_CONFIGURATION}")

message(STATUS "---------------------------------------------")
message(STATUS "feature preview options")
message(STATUS "---------------------------------------------")
message(
  STATUS
    "OTELCPP_WITH_ASYNC_EXPORT_PREVIEW: ${OTELCPP_WITH_ASYNC_EXPORT_PREVIEW}")
message(
  STATUS
    "OTELCPP_WITH_THREAD_INSTRUMENTATION_PREVIEW: ${OTELCPP_WITH_THREAD_INSTRUMENTATION_PREVIEW}"
)
message(
  STATUS
    "OTELCPP_WITH_METRICS_EXEMPLAR_PREVIEW: ${OTELCPP_WITH_METRICS_EXEMPLAR_PREVIEW}"
)
message(
  STATUS
    "OTELCPP_WITH_OTLP_GRPC_SSL_MTLS_PREVIEW: ${OTELCPP_WITH_OTLP_GRPC_SSL_MTLS_PREVIEW}"
)
message(
  STATUS
    "OTELCPP_WITH_OTLP_GRPC_CREDENTIAL_PREVIEW: ${OTELCPP_WITH_OTLP_GRPC_CREDENTIAL_PREVIEW}"
)
message(
  STATUS "OTELCPP_WITH_OTLP_RETRY_PREVIEW: ${OTELCPP_WITH_OTLP_RETRY_PREVIEW}")
message(
  STATUS
    "OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW: ${OTELCPP_WITH_METRICS_BOUND_INSTRUMENTS_PREVIEW}"
)
message(STATUS "---------------------------------------------")
message(STATUS "third-party options")
message(STATUS "---------------------------------------------")
message(STATUS "WITH_NLOHMANN_JSON: ${USE_NLOHMANN_JSON}")
message(STATUS "OTELCPP_WITH_CURL_LOGGING: ${OTELCPP_WITH_CURL_LOGGING}")
message(
  STATUS
    "OTELCPP_WITH_OTLP_HTTP_COMPRESSION: ${OTELCPP_WITH_OTLP_HTTP_COMPRESSION}")
message(STATUS "---------------------------------------------")
message(STATUS "examples and test options")
message(STATUS "---------------------------------------------")
message(STATUS "OTELCPP_WITH_BENCHMARK: ${OTELCPP_WITH_BENCHMARK}")
message(STATUS "OTELCPP_WITH_EXAMPLES: ${OTELCPP_WITH_EXAMPLES}")
message(STATUS "OTELCPP_WITH_EXAMPLES_HTTP: ${OTELCPP_WITH_EXAMPLES_HTTP}")
message(STATUS "OTELCPP_WITH_FUNC_TESTS: ${OTELCPP_WITH_FUNC_TESTS}")
message(
  STATUS
    "OTELCPP_BUILD_W3CTRACECONTEXT_TEST: ${OTELCPP_BUILD_W3CTRACECONTEXT_TEST}")
message(STATUS "OTELCPP_BUILD_TESTING: ${OTELCPP_BUILD_TESTING}")
message(STATUS "---------------------------------------------")
message(STATUS "versions")
message(STATUS "---------------------------------------------")
message(STATUS "CMake: ${CMAKE_VERSION}")
message(STATUS "GTest: ${GTest_VERSION} (${GTest_PROVIDER})")
message(STATUS "benchmark: ${benchmark_VERSION} (${benchmark_PROVIDER})")
if(OTELCPP_WITH_GSL)
  message(
    STATUS "Microsoft.GSL: ${Microsoft.GSL_VERSION} (${Microsoft.GSL_PROVIDER})"
  )
endif()
if(absl_FOUND)
  message(STATUS "Abseil: ${absl_VERSION}")
endif()
if(opentelemetry-proto_VERSION)
  message(
    STATUS
      "opentelemetry-proto: ${opentelemetry-proto_VERSION} (${opentelemetry-proto_PROVIDER})"
  )
endif()
if(Protobuf_VERSION)
  message(
    STATUS
      "Protobuf: ${Protobuf_VERSION} (${Protobuf_PROVIDER} - ${protobuf_lib_type})"
  )
endif()
if(gRPC_VERSION)
  message(STATUS "gRPC: ${gRPC_VERSION} (${gRPC_PROVIDER} - ${grpc_lib_type})")
endif()
if(CURL_VERSION)
  message(STATUS "CURL: ${CURL_VERSION} (${CURL_PROVIDER})")
endif()
if(ZLIB_VERSION)
  message(STATUS "ZLIB: ${ZLIB_VERSION} (${ZLIB_PROVIDER})")
endif()
if(USE_NLOHMANN_JSON)
  message(
    STATUS "nlohmann-json: ${nlohmann_json_VERSION} (${nlohmann_json_PROVIDER})"
  )
endif()
if(OTELCPP_WITH_PROMETHEUS)
  message(
    STATUS
      "prometheus-cpp: ${prometheus-cpp_VERSION} (${prometheus-cpp_PROVIDER})")
endif()
if(OTELCPP_WITH_OPENTRACING)
  message(
    STATUS "opentracing-cpp: ${OpenTracing_VERSION} (${OpenTracing_PROVIDER})")
endif()
if(ryml_VERSION)
  message(STATUS "ryml: ${ryml_VERSION} (${ryml_PROVIDER})")
endif()
message(STATUS "---------------------------------------------")

include("${opentelemetry-cpp_SOURCE_DIR}/cmake/otel-install-functions.cmake")

include(CMakePackageConfigHelpers)

if(OTELCPP_BUILD_DLL)
  if(NOT WIN32)
    message(FATAL_ERROR "Build DLL is only supported on Windows!")
  endif()
  if(NOT MSVC)
    message(WARNING "Build DLL is supposed to work with MSVC!")
  endif()
  if(OTELCPP_WITH_STL)
    message(
      WARNING "Build DLL with C++ STL could cause binary incompatibility!")
  endif()
  add_definitions(-DOPENTELEMETRY_BUILD_EXPORT_DLL)
endif()

add_subdirectory(api)

if(NOT OTELCPP_WITH_API_ONLY)
  if(OTELCPP_WITH_OPENTRACING)
    add_subdirectory(opentracing-shim)
  endif()

  add_subdirectory(sdk)
  add_subdirectory(ext)
  add_subdirectory(exporters)
  if(OTELCPP_WITH_RESOURCE_DETECTORS_PREVIEW)
    add_subdirectory(resource_detectors)
  endif()

  if(OTELCPP_BUILD_TESTING)
    add_subdirectory(test_common)
  endif()
  if(OTELCPP_WITH_EXAMPLES)
    add_subdirectory(examples)
  endif()
  if(OTELCPP_WITH_FUNC_TESTS)
    add_subdirectory(functional)
  endif()

  include(
    "${opentelemetry-cpp_SOURCE_DIR}/cmake/opentelemetry-build-external-component.cmake"
  )
endif()

include("${opentelemetry-cpp_SOURCE_DIR}/cmake/patch-imported-config.cmake")

if(OTELCPP_INSTALL)
  # Install the cmake config and version files
  otel_install_cmake_config()

  # Install the components and associated files
  otel_install_components()

  # Install the thirdparty dependency definition file
  otel_install_thirdparty_definitions()

  if(OTELCPP_BUILD_PACKAGE)
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
