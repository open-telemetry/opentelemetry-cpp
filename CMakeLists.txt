cmake_minimum_required(VERSION 3.1)

# See https://cmake.org/cmake/help/v3.3/policy/CMP0057.html required by certain
# versions of gtest
cmake_policy(SET CMP0057 NEW)

project(opentelemetry-cpp)

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

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

option(WITH_STL "Whether to use Standard Library for C++latest features" OFF)

option(WITH_ABSEIL "Whether to use Abseil for C++latest features" OFF)

if(WITH_ABSEIL)
  add_definitions(-DHAVE_ABSEIL)
  find_package(absl CONFIG REQUIRED)

  # Abseil headers-only lib is needed for absl::variant to work vs2015.
  # `nostd::mpark::variant` is not compiling in vs2015.
  set(CORE_RUNTIME_LIBS absl::any absl::base absl::bits absl::city)

  # target_link_libraries(main PRIVATE absl::any absl::base absl::bits
  # absl::city)
endif()

if(WITH_STL)
  # Require at least C++17. C++20 is needed to avoid gsl::span
  if(CMAKE_MINOR_VERSION VERSION_GREATER "3.18")
    # Ask for 20, may get anything below
    set(CMAKE_CXX_STANDARD 20)
  else()
    # Ask for 17, may get anything below
    set(CMAKE_CXX_STANDARD 17)
  endif()

  # Guidelines Support Library path. Used if we are not on not get C++20.
  #
  # TODO: respect WITH_ABSEIL as alternate implementation of std::span
  set(GSL_DIR third_party/ms-gsl)
  include_directories(${GSL_DIR}/include)

  # Optimize for speed to reduce the hops
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
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} /Zc:__cplusplus ${MSVC_CXX_OPT_FLAG}")
  endif()
endif()

option(WITH_OTLP "Whether to include the OpenTelemetry Protocol in the SDK" OFF)
option(WITH_ZIPKIN "Whether to include the Zipkin exporter in the SDK" OFF)

option(WITH_PROMETHEUS "Whether to include the Prometheus Client in the SDK"
       OFF)

option(WITH_ELASTICSEARCH
       "Whether to include the Elasticsearch Client in the SDK" OFF)

option(WITH_JAEGER "Whether to include the Jaeger exporter" OFF)

option(BUILD_TESTING "Whether to enable tests" ON)
if(WIN32)
  if(BUILD_TESTING)
    if(MSVC)
      # GTest bug: https://github.com/google/googletest/issues/860
      add_compile_options(/wd4275)
    endif()
  endif()
  option(WITH_ETW "Whether to include the ETW Exporter in the SDK" ON)
  if(WITH_ETW)
    add_definitions(-DHAVE_MSGPACK)
  endif(WITH_ETW)
endif(WIN32)

option(
  WITH_API_ONLY
  "Only build the API (use as a header-only library). Overrides WITH_EXAMPLES"
  OFF)
option(WITH_EXAMPLES "Whether to build examples" ON)

option(WITH_METRICS_PREVIEW "Whether to build metrics preview" OFF)

if(WITH_METRICS_PREVIEW)
  add_definitions(-DENABLE_METRICS_PREVIEW)
endif()

option(WITH_LOGS_PREVIEW "Whether to build logs preview" OFF)

if(WITH_LOGS_PREVIEW)
  add_definitions(-DENABLE_LOGS_PREVIEW)
endif()

find_package(Threads)

function(install_windows_deps)
  # Bootstrap vcpkg from CMake and auto-install deps in case if we are missing
  # deps on Windows
  message("Installing build tools and dependencies...")
  execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/setup-buildtools.cmd)
  set(CMAKE_TOOLCHAIN_FILE
      ${CMAKE_CURRENT_SOURCE_DIR}/tools/vcpkg/scripts/buildsystems/vcpkg.cmake
      PARENT_SCOPE)
endfunction()

if(MSVC)
  # Options for Visual C++ compiler: /Zc:__cplusplus - report an updated value
  # for recent C++ language standards. Without this option MSVC returns the
  # value of __cplusplus="199711L"
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")

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

if(WITH_OTLP)
  set(protobuf_MODULE_COMPATIBLE ON)
  find_package(Protobuf)
  find_package(gRPC)
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
    if(NOT gRPC_FOUND)
      find_package(gRPC REQUIRED)
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
  message("PROTOBUF_PROTOC_EXECUTABLE=${PROTOBUF_PROTOC_EXECUTABLE}")

  include(cmake/opentelemetry-proto.cmake)
endif()

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

include(CTest)
if(BUILD_TESTING)
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
    find_package(GTest)
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
  include_directories(sdk/include)
  include_directories(sdk)
  include_directories(ext/include)

  add_subdirectory(sdk)
  add_subdirectory(exporters)
  if(WITH_EXAMPLES)
    add_subdirectory(examples)
  endif()
  add_subdirectory(ext)
endif()

# Add nlohmann/json submodule to include directories
include_directories(third_party/nlohmann-json/single_include)

# Export cmake config and support find_packages(opentelemetry-cpp CONFIG) Write
# config file for find_packages(opentelemetry-cpp CONFIG)
set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/opentelemetry-cpp-config.cmake.in"
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
