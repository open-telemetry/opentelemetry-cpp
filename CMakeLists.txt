cmake_minimum_required(VERSION 3.1)

# See https://cmake.org/cmake/help/v3.3/policy/CMP0057.html required by certain
# versions of gtest
cmake_policy(SET CMP0057 NEW)

project(opentelemetry-cpp)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

option(WITH_STL
       "Whether to use Standard Library for C++latest features" OFF)

if(WITH_STL)
  # We require at least C++17. C++20 is optimal to avoid gsl::span
  add_definitions(-DHAVE_CPP_STDLIB -DHAVE_GSL)
  # We ask for 20, but we may get C++17
  set(CMAKE_CXX_STANDARD 20)
  # Guidelines Support Library path
  set(GSL_DIR third_party/ms-gsl)
  include_directories(${GSL_DIR}/include)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_SPEED "/O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus ${CMAKE_CXX_FLAGS_SPEED}")
  endif()
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
  endif()
endif()

option(WITH_OTPROTOCOL
       "Whether to include the OpenTelemetry Protocol in the SDK" OFF)

set(WITH_PROTOBUF OFF)
if(WITH_OTPROTOCOL)
  set(WITH_PROTOBUF ON)
endif()

include(CTest)

find_package(Threads)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Options for Visual C++ compiler: /Zc:__cplusplus - report an updated value
  # for recent C++ language standards. Without this option MSVC returns the
  # value of __cplusplus="199711L"
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
endif()

if(WITH_PROTOBUF)
  set(protobuf_MODULE_COMPATIBLE ON)
  find_package(Protobuf CONFIG NAMES protobuf)
  # Older versions of protobuf don't use cmake config files.
  if(NOT protobuf_FOUND)
    find_package(Protobuf REQUIRED)
  endif()
endif()

if(WITH_OTPROTOCOL)
  include(third_party/opentelemetry-proto/Protobuf.cmake)
endif()

if(BUILD_TESTING)
  find_package(GTest REQUIRED)
  find_package(benchmark REQUIRED)
  include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})
endif()

include_directories(api/include)
add_subdirectory(api)
include_directories(sdk/include)
include_directories(sdk)
add_subdirectory(sdk)
include_directories(.)
add_subdirectory(exporters)
add_subdirectory(examples)
