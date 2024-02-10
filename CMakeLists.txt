# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.1)

# See https://cmake.org/cmake/help/v3.3/policy/CMP0057.html required by certain
# versions of gtest
cmake_policy(SET CMP0057 NEW)

# See https://cmake.org/cmake/help/v3.12/policy/CMP0074.html required by certain
# version of zlib which CURL depends on.
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.12")
  cmake_policy(SET CMP0074 NEW)
endif()

project(opentelemetry-cpp)

# Mark variables as used so cmake doesn't complain about them
mark_as_advanced(CMAKE_TOOLCHAIN_FILE)

# Prefer cmake CONFIG to auto resolve dependencies.
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

# Don't use customized cmake modules if vcpkg is used to resolve dependence.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/third_party_release")
  file(STRINGS "${CMAKE_SOURCE_DIR}/third_party_release" third_party_tags)
  foreach(third_party ${third_party_tags})
    string(REGEX REPLACE "^[ ]+" "" third_party ${third_party})
    string(REGEX MATCH "^[^=]+" third_party_name ${third_party})
    string(REPLACE "${third_party_name}=" "" third_party_tag ${third_party})
    set(${third_party_name} "${third_party_tag}")
  endforeach()
endif()

if(DEFINED ENV{ARCH})
  # Architecture may be specified via ARCH environment variable
  set(ARCH $ENV{ARCH})
else()
  # Autodetection logic that populates ARCH variable
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "amd64.*|x86_64.*|AMD64.*")
    # Windows may report AMD64 even if target is 32-bit
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(ARCH x64)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
      set(ARCH x86)
    endif()
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i686.*|i386.*|x86.*")
    # Windows may report x86 even if target is 64-bit
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(ARCH x64)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
      set(ARCH x86)
    endif()
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES
         "^(aarch64.*|AARCH64.*|arm64.*|ARM64.*)")
    set(ARCH arm64)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm.*|ARM.*)")
    set(ARCH arm)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64le")
    set(ARCH ppc64le)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64")
    set(ARCH ppc64)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(mips.*|MIPS.*)")
    set(ARCH mips)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(riscv.*|RISCV.*)")
    set(ARCH riscv)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(s390x.*|S390X.*)")
    set(ARCH s390x)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(sparc.*|SPARC.*)")
    set(ARCH sparc)
  else()
    message(
      FATAL_ERROR
        "opentelemetry-cpp: unrecognized target processor ${CMAKE_SYSTEM_PROCESSOR} configuration!"
    )
  endif()
endif()
message(STATUS "Building for architecture ARCH=${ARCH}")

# Autodetect vcpkg toolchain
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

if(VCPKG_CHAINLOAD_TOOLCHAIN_FILE)
  include("${VCPKG_CHAINLOAD_TOOLCHAIN_FILE}")
endif()

option(WITH_ABI_VERSION_1 "ABI version 1" ON)
option(WITH_ABI_VERSION_2 "EXPERIMENTAL: ABI version 2 preview" OFF)

file(READ "${CMAKE_CURRENT_LIST_DIR}/api/include/opentelemetry/version.h"
     OPENTELEMETRY_CPP_HEADER_VERSION_H)

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
  if(OPENTELEMETRY_CPP_HEADER_VERSION_H MATCHES
     "OPENTELEMETRY_ABI_VERSION_NO[ \t\r\n]+\"?([0-9]+)\"?")
    math(EXPR OPENTELEMETRY_ABI_VERSION_NO ${CMAKE_MATCH_1})
  else()
    message(
      FATAL_ERROR
        "OPENTELEMETRY_ABI_VERSION_NO not found on ${CMAKE_CURRENT_LIST_DIR}/api/include/opentelemetry/version.h"
    )
  endif()
endif()

message(STATUS "OPENTELEMETRY_ABI_VERSION_NO=${OPENTELEMETRY_ABI_VERSION_NO}")

if(OPENTELEMETRY_CPP_HEADER_VERSION_H MATCHES
   "OPENTELEMETRY_VERSION[ \t\r\n]+\"?([^\"]+)\"?")
  set(OPENTELEMETRY_VERSION ${CMAKE_MATCH_1})
else()
  message(
    FATAL_ERROR
      "OPENTELEMETRY_VERSION not found on ${CMAKE_CURRENT_LIST_DIR}/api/include/opentelemetry/version.h"
  )
endif()

message(STATUS "OPENTELEMETRY_VERSION=${OPENTELEMETRY_VERSION}")

option(WITH_NO_DEPRECATED_CODE "Do not include deprecated code" OFF)

set(WITH_STL
    "OFF"
    CACHE STRING "Which version of the Standard Library for C++ to use")

option(WITH_GSL
       "Whether to use Guidelines Support Library for C++ latest features" OFF)

option(WITH_ABSEIL "Whether to use Abseil for C++latest features" OFF)

set(OPENTELEMETRY_INSTALL_default ON)
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(OPENTELEMETRY_INSTALL_default OFF)
endif()
option(OPENTELEMETRY_INSTALL "Whether to install opentelemetry targets"
       ${OPENTELEMETRY_INSTALL_default})

include("${PROJECT_SOURCE_DIR}/cmake/tools.cmake")

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

if(DEFINED WITH_OTLP)
  message(
    FATAL_ERROR
      "WITH_OTLP is deprecated. Please set either WITH_OTLP_GRPC=ON, WITH_OTLP_HTTP=ON, or both to ON."
  )
endif()

option(WITH_OTLP_GRPC "Whether to include the OTLP gRPC exporter in the SDK"
       OFF)

option(WITH_OTLP_HTTP "Whether to include the OTLP http exporter in the SDK"
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

set(OTELCPP_PROTO_PATH
    ""
    CACHE PATH "Path to opentelemetry-proto")

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

if(DEFINED WITH_LOGS_PREVIEW)
  message(WARNING "WITH_LOGS_PREVIEW is removed because logs signal is stable")
endif()

option(WITH_ASYNC_EXPORT_PREVIEW "Whether to enable async export" OFF)

# Exemplar specs status is experimental, so behind feature flag by default
option(WITH_METRICS_EXEMPLAR_PREVIEW
       "Whether to enable exemplar within metrics" OFF)

#
# Verify options dependencies
#

if(WITH_EXAMPLES_HTTP AND NOT WITH_EXAMPLES)
  message(FATAL_ERROR "WITH_EXAMPLES_HTTP=ON requires WITH_EXAMPLES=ON")
endif()

find_package(Threads)

function(install_windows_deps)
  # Bootstrap vcpkg from CMake and auto-install deps in case if we are missing
  # deps on Windows. Respect the target architecture variable.
  set(VCPKG_TARGET_ARCHITECTURE
      ${ARCH}
      PARENT_SCOPE)
  message("Installing build tools and dependencies...")
  set(ENV{ARCH} ${ARCH})
  execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/setup-buildtools.cmd)
  set(CMAKE_TOOLCHAIN_FILE
      ${CMAKE_CURRENT_SOURCE_DIR}/tools/vcpkg/scripts/buildsystems/vcpkg.cmake
      CACHE FILEPATH "")
  message(
    "Make sure that vcpkg.cmake is set as the CMAKE_TOOLCHAIN_FILE at the START of the cmake build process!
    Can be command-line arg (cmake -DCMAKE_TOOLCHAIN_FILE=...) or set in your editor of choice."
  )

endfunction()

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
  # When using vcpkg, all targets build with the same runtime
  if(VCPKG_TOOLCHAIN)
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<STREQUAL:${VCPKG_CRT_LINKAGE},dynamic>:DLL>"
        CACHE STRING "")
  endif()
endif()

# include GNUInstallDirs before include cmake/opentelemetry-proto.cmake because
# opentelemetry-proto installs targets to the location variables defined in
# GNUInstallDirs.
include(GNUInstallDirs)

if(WITH_PROMETHEUS)
  find_package(prometheus-cpp CONFIG QUIET)
  if(NOT prometheus-cpp_FOUND)
    message("Trying to use local prometheus-cpp from submodule")
    if(EXISTS ${PROJECT_SOURCE_DIR}/third_party/prometheus-cpp/.git)
      set(SAVED_ENABLE_TESTING ${ENABLE_TESTING})
      set(ENABLE_TESTING OFF)
      add_subdirectory(third_party/prometheus-cpp)
      set(ENABLE_TESTING ${SAVED_ENABLE_TESTING})
    else()
      message(
        FATAL_ERROR
          "\nprometheus-cpp package was not found. Please either provide it manually or clone with submodules. "
          "To initialize, fetch and checkout any nested submodules, you can use the following command:\n"
          "git submodule update --init --recursive")
    endif()
  else()
    message("Using external prometheus-cpp")
  endif()
endif()

if(WITH_ABSEIL)
  find_package(absl CONFIG REQUIRED)
endif()

if(WITH_OTLP_GRPC OR WITH_OTLP_HTTP)
  find_package(Protobuf)
  if(Protobuf_VERSION AND Protobuf_VERSION VERSION_GREATER_EQUAL "3.22.0")
    if(NOT WITH_ABSEIL)
      message(
        FATAL_ERROR
          "Protobuf 3.22 or upper require abseil-cpp(Recommended version: 20230125 or upper)"
      )
    endif()
  endif()

  if(WITH_OTLP_GRPC)
    find_package(gRPC)
  endif()
  if((NOT Protobuf_FOUND AND NOT PROTOBUF_FOUND) OR (NOT gRPC_FOUND))
    if(WIN32 AND (NOT DEFINED CMAKE_TOOLCHAIN_FILE))
      install_windows_deps()
    endif()

    if(WIN32 AND (NOT DEFINED CMAKE_TOOLCHAIN_FILE))
      message(STATUS_FATAL "Windows dependency installation failed!")
    endif()
    if(WIN32)
      include(${CMAKE_TOOLCHAIN_FILE})
    endif()

    if(NOT Protobuf_FOUND AND NOT PROTOBUF_FOUND)
      find_package(Protobuf REQUIRED)
    endif()
    if(NOT gRPC_FOUND AND WITH_OTLP_GRPC)
      find_package(gRPC)
    endif()
    if(WIN32)
      # Always use x64 protoc.exe
      if(NOT EXISTS "${Protobuf_PROTOC_EXECUTABLE}")
        set(Protobuf_PROTOC_EXECUTABLE
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/vcpkg/packages/protobuf_x64-windows/tools/protobuf/protoc.exe
        )
      endif()
    endif()
  endif()
  # Latest Protobuf imported targets and without legacy module support
  if(TARGET protobuf::protoc)
    project_build_tools_get_imported_location(PROTOBUF_PROTOC_EXECUTABLE
                                              protobuf::protoc)
    # If protobuf::protoc is not a imported target, then we use the target
    # directly for fallback
    if(NOT PROTOBUF_PROTOC_EXECUTABLE)
      set(PROTOBUF_PROTOC_EXECUTABLE protobuf::protoc)
    endif()
  elseif(Protobuf_PROTOC_EXECUTABLE)
    # Some versions of FindProtobuf.cmake uses mixed case instead of uppercase
    set(PROTOBUF_PROTOC_EXECUTABLE ${Protobuf_PROTOC_EXECUTABLE})
  endif()
  include(CMakeDependentOption)

  message(STATUS "PROTOBUF_PROTOC_EXECUTABLE=${PROTOBUF_PROTOC_EXECUTABLE}")
  include(cmake/opentelemetry-proto.cmake)
endif()

#
# Do we need HTTP CLIENT CURL ?
#

if(WITH_OTLP_HTTP
   OR WITH_ELASTICSEARCH
   OR WITH_ZIPKIN
   OR BUILD_W3CTRACECONTEXT_TEST
   OR WITH_EXAMPLES_HTTP)
  set(WITH_HTTP_CLIENT_CURL ON)
else()
  set(WITH_HTTP_CLIENT_CURL OFF)
endif()

#
# Do we need CURL ?
#

if((NOT WITH_API_ONLY) AND WITH_HTTP_CLIENT_CURL)
  # No specific version required.
  find_package(CURL REQUIRED)
  message(STATUS "Found CURL: ${CURL_LIBRARIES}, version ${CURL_VERSION}")
endif()

#
# Do we need NLOHMANN_JSON ?
#

if(WITH_ELASTICSEARCH
   OR WITH_ZIPKIN
   OR WITH_OTLP_HTTP
   OR BUILD_W3CTRACECONTEXT_TEST
   OR WITH_ETW)
  set(USE_NLOHMANN_JSON ON)
else()
  set(USE_NLOHMANN_JSON OFF)
endif()

if((NOT WITH_API_ONLY) AND USE_NLOHMANN_JSON)
  include(cmake/nlohmann-json.cmake)
endif()

if(OTELCPP_MAINTAINER_MODE)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message("Building with gcc in maintainer mode.")

    add_compile_options(-Wall)
    add_compile_options(-Werror)
    add_compile_options(-Wextra)

    # Tested with GCC 9.4 on github.
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 9.4)
      message("Building with additional warnings for gcc.")

      # Relaxed warnings

      # Enforced warnings

      # C++ options only
      add_compile_options($<$<STREQUAL:$<COMPILE_LANGUAGE>,CXX>:-Wextra-semi>)
      add_compile_options(
        $<$<STREQUAL:$<COMPILE_LANGUAGE>,CXX>:-Woverloaded-virtual>)
      add_compile_options(
        $<$<STREQUAL:$<COMPILE_LANGUAGE>,CXX>:-Wsuggest-override>)

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
    message("Building with clang in maintainer mode.")

    add_compile_options(-Wall)
    add_compile_options(-Werror)
    add_compile_options(-Wextra)

    # Tested with Clang 11.0 on github.
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 11.0)
      message("Building with additional warnings for clang.")

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
    endif()
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message("Building with msvc in maintainer mode.")

    add_compile_options(/WX)
    add_compile_options(/W4)

    # Relaxed warnings
    add_compile_options(/wd4100)
    add_compile_options(/wd4125)
    add_compile_options(/wd4566)
    add_compile_options(/wd4127)
    add_compile_options(/wd4512)
    add_compile_options(/wd4267)

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

include(CTest)
if(BUILD_TESTING)
  if(EXISTS ${CMAKE_BINARY_DIR}/lib/libgtest.a)
    # Prefer GTest from build tree. GTest is not always working with
    # CMAKE_PREFIX_PATH
    set(GTEST_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googlemock/include)
    if(TARGET gtest)
      set(GTEST_BOTH_LIBRARIES gtest gtest_main)
    else()
      set(GTEST_BOTH_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libgtest.a
                               ${CMAKE_BINARY_DIR}/lib/libgtest_main.a)
    endif()
  elseif(WIN32)
    # Make sure we are always bootsrapped with vcpkg on Windows
    find_package(GTest)
    if(NOT (GTEST_FOUND OR GTest_FOUND))
      if(DEFINED CMAKE_TOOLCHAIN_FILE)
        message(
          FATAL_ERROR
            "Pleaes install GTest with the CMAKE_TOOLCHAIN_FILE at ${CMAKE_TOOLCHAIN_FILE}"
        )
      else()
        install_windows_deps()
        include(${CMAKE_TOOLCHAIN_FILE})
        find_package(GTest REQUIRED)
      endif()
    endif()
  else()
    # Prefer GTest installed by OS distro, brew or vcpkg package manager
    find_package(GTest REQUIRED)
  endif()
  if(NOT GTEST_BOTH_LIBRARIES)
    # New GTest package names
    if(TARGET GTest::gtest)
      set(GTEST_BOTH_LIBRARIES GTest::gtest GTest::gtest_main)
    elseif(TARGET GTest::GTest)
      set(GTEST_BOTH_LIBRARIES GTest::GTest GTest::Main)
    endif()
  endif()
  if(GTEST_INCLUDE_DIRS)
    include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})
  endif()
  message("GTEST_INCLUDE_DIRS   = ${GTEST_INCLUDE_DIRS}")
  message("GTEST_BOTH_LIBRARIES = ${GTEST_BOTH_LIBRARIES}")
  enable_testing()
  if(WITH_BENCHMARK)
    # Benchmark respects the CMAKE_PREFIX_PATH
    find_package(benchmark CONFIG REQUIRED)
  endif()
endif()

include(CMakePackageConfigHelpers)

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

include_directories(api/include)

add_subdirectory(api)

if(WITH_OPENTRACING)
  find_package(OpenTracing CONFIG QUIET)
  if(NOT OpenTracing_FOUND)
    set(OPENTRACING_DIR "third_party/opentracing-cpp")
    message("Trying to use local ${OPENTRACING_DIR} from submodule")
    if(EXISTS "${PROJECT_SOURCE_DIR}/${OPENTRACING_DIR}/.git")
      set(SAVED_BUILD_TESTING ${BUILD_TESTING})
      set(BUILD_TESTING OFF)
      add_subdirectory(${OPENTRACING_DIR})
      set(BUILD_TESTING ${SAVED_BUILD_TESTING})
    else()
      message(
        FATAL_ERROR
          "\nopentracing-cpp package was not found. Please either provide it manually or clone with submodules. "
          "To initialize, fetch and checkout any nested submodules, you can use the following command:\n"
          "git submodule update --init --recursive")
    endif()
  else()
    message("Using external opentracing-cpp")
  endif()
  add_subdirectory(opentracing-shim)
endif()

if(NOT WITH_API_ONLY)
  set(BUILD_TESTING ${BUILD_TESTING})
  include_directories(sdk/include)
  include_directories(sdk)
  include_directories(ext/include)

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

include(cmake/opentelemetry-build-external-component.cmake)
include(cmake/patch-imported-config.cmake)

if(OPENTELEMETRY_INSTALL)
  # Export cmake config and support find_packages(opentelemetry-cpp CONFIG)
  # Write config file for find_packages(opentelemetry-cpp CONFIG)
  set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
  configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/opentelemetry-cpp-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    PATH_VARS OPENTELEMETRY_ABI_VERSION_NO OPENTELEMETRY_VERSION PROJECT_NAME
              INCLUDE_INSTALL_DIR CMAKE_INSTALL_LIBDIR
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)

  # Write version file for find_packages(opentelemetry-cpp CONFIG)
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake"
    VERSION ${OPENTELEMETRY_VERSION}
    COMPATIBILITY ExactVersion)

  install(
    FILES
      "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

  # Export all components
  export(
    EXPORT "${PROJECT_NAME}-target"
    NAMESPACE "${PROJECT_NAME}::"
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-target.cmake"
  )
  install(
    EXPORT "${PROJECT_NAME}-target"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

  if(BUILD_PACKAGE)
    include(cmake/package.cmake)
    include(CPack)
  endif()
endif()
