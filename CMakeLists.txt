cmake_minimum_required(VERSION 3.1)

# See https://cmake.org/cmake/help/v3.3/policy/CMP0057.html required by certain
# versions of gtest
cmake_policy(SET CMP0057 NEW)

# See https://cmake.org/cmake/help/v3.12/policy/CMP0074.html required by certain
# version of zlib which is dependeded by cURL.
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.12")
  cmake_policy(SET CMP0074 NEW)
endif()

project(opentelemetry-cpp)

# Mark variables as used so cmake doesn't complain about them
mark_as_advanced(CMAKE_TOOLCHAIN_FILE)

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
  else()
    message(
      FATAL_ERROR
        "opentelemetry-cpp: unrecognized target processor configuration!")
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

file(READ "${CMAKE_CURRENT_LIST_DIR}/api/include/opentelemetry/version.h"
     OPENTELEMETRY_CPP_HEADER_VERSION_H)
if(OPENTELEMETRY_CPP_HEADER_VERSION_H MATCHES
   "OPENTELEMETRY_ABI_VERSION_NO[ \t\r\n]+\"?([0-9]+)\"?")
  math(EXPR OPENTELEMETRY_ABI_VERSION_NO ${CMAKE_MATCH_1})
else()
  message(
    FATAL_ERROR
      "OPENTELEMETRY_ABI_VERSION_NO not found on ${CMAKE_CURRENT_LIST_DIR}/api/include/opentelemetry/version.h"
  )
endif()
if(OPENTELEMETRY_CPP_HEADER_VERSION_H MATCHES
   "OPENTELEMETRY_VERSION[ \t\r\n]+\"?([^\"]+)\"?")
  set(OPENTELEMETRY_VERSION ${CMAKE_MATCH_1})
else()
  message(
    FATAL_ERROR
      "OPENTELEMETRY_VERSION not found on ${CMAKE_CURRENT_LIST_DIR}/api/include/opentelemetry/version.h"
  )
endif()

option(WITH_STL "Whether to use Standard Library for C++ latest features" OFF)
option(WITH_GSL
       "Whether to use Guidelines Support Library for C++ latest features" OFF)

option(WITH_ABSEIL "Whether to use Abseil for C++latest features" OFF)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  if(WITH_STL)
    # Require at least C++17. C++20 is needed to avoid gsl::span
    if(CMAKE_VERSION VERSION_GREATER 3.11.999)
      # Ask for 20, may get anything below
      set(CMAKE_CXX_STANDARD 20)
    else()
      # Ask for 17, may get anything below
      set(CMAKE_CXX_STANDARD 17)
    endif()
  else()
    set(CMAKE_CXX_STANDARD 11)
  endif()
endif()

if(WITH_STL)
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

option(WITH_OTLP "Whether to include the OpenTelemetry Protocol in the SDK" OFF)
option(WITH_ZIPKIN "Whether to include the Zipkin exporter in the SDK" OFF)

option(WITH_PROMETHEUS "Whether to include the Prometheus Client in the SDK"
       OFF)

option(WITH_ELASTICSEARCH
       "Whether to include the Elasticsearch Client in the SDK" OFF)

option(WITH_ZPAGES "Whether to include the Zpages Server in the SDK" OFF)

option(WITH_JAEGER "Whether to include the Jaeger exporter" OFF)

option(WITH_NO_GETENV "Whether the platform supports environment variables" OFF)

option(BUILD_TESTING "Whether to enable tests" ON)

option(BUILD_W3CTRACECONTEXT_TEST "Whether to build w3c trace context" OFF)

if(WIN32)
  if(BUILD_TESTING)
    if(MSVC)
      # GTest bug: https://github.com/google/googletest/issues/860
      add_compile_options(/wd4275)
    endif()
  endif()
  option(WITH_ETW "Whether to include the ETW Exporter in the SDK" ON)
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

option(WITH_METRICS_PREVIEW "Whether to build metrics preview" OFF)
option(WITH_LOGS_PREVIEW "Whether to build logs preview" OFF)

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
      PARENT_SCOPE)
endfunction()

if(WITH_JAEGER)
  find_package(Thrift QUIET)
  if(Thrift_FOUND)
    find_package(Boost REQUIRED)
    include_directories(${Boost_INCLUDE_DIR})
  else()
    # Install Thrift and propagate via vcpkg toolchain file
    if(WIN32 AND (NOT DEFINED CMAKE_TOOLCHAIN_FILE))
      install_windows_deps()
    endif()
  endif()
endif()

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

if((NOT WITH_API_ONLY)
   AND (WITH_ELASTICSEARCH
        OR WITH_ZIPKIN
        OR WITH_OTLP
        OR WITH_OTLP_HTTP
        OR WITH_ZPAGES
        OR BUILD_W3CTRACECONTEXT_TEST
        OR WITH_ETW
       ))
  # nlohmann_json package is required for most SDK build configurations
  find_package(nlohmann_json QUIET)
  set(nlohmann_json_clone FALSE)
  if(nlohmann_json_FOUND)
    message("Using external nlohmann::json")
  elseif(EXISTS ${PROJECT_SOURCE_DIR}/.git
         AND EXISTS
             ${PROJECT_SOURCE_DIR}/third_party/nlohmann-json/CMakeLists.txt)
    message("Trying to use local nlohmann::json from submodule")
    set(JSON_BuildTests
        OFF
        CACHE INTERNAL "")
    set(JSON_Install
        ON
        CACHE INTERNAL "")
    # This option allows to link nlohmann_json::nlohmann_json target
    add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/nlohmann-json)
    # This option allows to add header to include directories
    include_directories(
      ${PROJECT_SOURCE_DIR}/third_party/nlohmann-json/single_include)
  else()
    set(nlohmann_json_clone TRUE)
    include(cmake/nlohmann-json.cmake)
    message("\nnlohmann_json package was not found. Cloning from github")
  endif()
endif()

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

if(WITH_OTLP)
  set(protobuf_MODULE_COMPATIBLE ON)
  find_package(Protobuf)
  if(WITH_OTLP_GRPC OR (NOT DEFINED WITH_OTLP_GRPC AND NOT DEFINED
                                                       CACHE{WITH_OTLP_GRPC}))
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
    if(NOT gRPC_FOUND
       AND (WITH_OTLP_GRPC OR (NOT DEFINED WITH_OTLP_GRPC
                               AND NOT DEFINED CACHE{WITH_OTLP_GRPC})))
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
    # Latest Protobuf uses mixed case instead of uppercase
    if(Protobuf_PROTOC_EXECUTABLE)
      set(PROTOBUF_PROTOC_EXECUTABLE ${Protobuf_PROTOC_EXECUTABLE})
    endif()
  endif()
  include(CMakeDependentOption)
  if(WITH_OTLP_HTTP OR (NOT DEFINED WITH_OTLP_HTTP AND NOT DEFINED
                                                       CACHE{WITH_OTLP_HTTP}))
    find_package(CURL)
  endif()

  cmake_dependent_option(
    WITH_OTLP_GRPC "Whether to include the OTLP gRPC exporter in the SDK" ON
    "gRPC_FOUND" OFF)
  cmake_dependent_option(
    WITH_OTLP_HTTP "Whether to include the OTLP http exporter in the SDK" ON
    "CURL_FOUND" OFF)

  message(STATUS "PROTOBUF_PROTOC_EXECUTABLE=${PROTOBUF_PROTOC_EXECUTABLE}")
  include(cmake/opentelemetry-proto.cmake)
endif()

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

include(CTest)
if(BUILD_TESTING)
  add_definitions(-DENABLE_TEST)
  if(EXISTS ${CMAKE_BINARY_DIR}/lib/libgtest.a)
    # Prefer GTest from build tree. GTest is not always working with
    # CMAKE_PREFIX_PATH
    set(GTEST_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googletest/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/googlemock/include)
    set(GTEST_BOTH_LIBRARIES
        ${CMAKE_BINARY_DIR}/lib/libgtest.a
        ${CMAKE_BINARY_DIR}/lib/libgtest_main.a
        ${CMAKE_BINARY_DIR}/lib/libgmock.a)
  elseif(WIN32)
    # Make sure we are always bootsrapped with vcpkg on Windows
    find_package(GTest REQUIRED)
    if(NOT (GTEST_FOUND OR GTest_FOUND))
      install_windows_deps()
      find_package(GTest REQUIRED)
    endif()
  else()
    # Prefer GTest installed by OS distro, brew or vcpkg package manager
    find_package(GTest REQUIRED)
  endif()
  include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})
  message("GTEST_INCLUDE_DIRS   = ${GTEST_INCLUDE_DIRS}")
  message("GTEST_BOTH_LIBRARIES = ${GTEST_BOTH_LIBRARIES}")
  enable_testing()
  # Benchmark respects the CMAKE_PREFIX_PATH
  find_package(benchmark CONFIG REQUIRED)
endif()

include(CMakePackageConfigHelpers)

include_directories(api/include)

add_subdirectory(api)

if(NOT WITH_API_ONLY)
  set(BUILD_TESTING ${BUILD_TESTING})
  include_directories(sdk/include)
  include_directories(sdk)
  include_directories(ext/include)

  add_subdirectory(sdk)
  add_subdirectory(ext)
  add_subdirectory(exporters)
  if(WITH_EXAMPLES)
    add_subdirectory(examples)
  endif()
endif()

# Export cmake config and support find_packages(opentelemetry-cpp CONFIG) Write
# config file for find_packages(opentelemetry-cpp CONFIG)
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
