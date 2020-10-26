cmake_minimum_required(VERSION 3.1)

# See https://cmake.org/cmake/help/v3.3/policy/CMP0057.html required by certain
# versions of gtest
cmake_policy(SET CMP0057 NEW)

project(opentelemetry-cpp)

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
  add_definitions(-DHAVE_CPP_STDLIB -DHAVE_GSL)

  # Ask for 20, but may get C++17
  set(CMAKE_CXX_STANDARD 20)

  # Guidelines Support Library path. Used if we are not on not get C++20.
  # 
  # TODO: respect WITH_ABSEIL as alternate implementation of std::span
  set(GSL_DIR third_party/ms-gsl)
  include_directories(${GSL_DIR}/include)

  # Optimize for speed to reduce the hops
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_SPEED "/O2")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} /Zc:__cplusplus ${CMAKE_CXX_FLAGS_SPEED}")
  endif()

endif()

option(WITH_OTPROTOCOL
       "Whether to include the OpenTelemetry Protocol in the SDK" OFF)

option(WITH_PROMETHEUS "Whether to include the Prometheus Client in the SDK"
       OFF)

option(WITH_TESTS "Whether to enable tests" ON)
option(WITH_EXAMPLES "Whether to build examples" ON)

set(WITH_PROTOBUF OFF)
if(WITH_OTPROTOCOL)
  set(WITH_PROTOBUF ON)
endif()

if(WITH_TESTS)
  include(CTest)
endif()

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
if(WITH_EXAMPLES)
  add_subdirectory(examples)
endif()
add_subdirectory(ext)
