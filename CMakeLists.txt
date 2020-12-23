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
    set(CMAKE_CXX_FLAGS_SPEED "/O2")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} /Zc:__cplusplus ${CMAKE_CXX_FLAGS_SPEED}")
  endif()
endif()

option(WITH_OTLP "Whether to include the OpenTelemetry Protocol in the SDK" OFF)

option(WITH_PROMETHEUS "Whether to include the Prometheus Client in the SDK"
       OFF)

option(WITH_ELASTICSEARCH
       "Whether to include the Elasticsearch Client in the SDK" OFF)

option(BUILD_TESTING "Whether to enable tests" ON)
option(WITH_EXAMPLES "Whether to build examples" ON)

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
endif()

if(WITH_OTLP)
  set(protobuf_MODULE_COMPATIBLE ON)
  find_package(Protobuf)
  find_package(gRPC)
  if((NOT protobuf_FOUND) OR (NOT gRPC_FOUND))
    if(WIN32 AND (NOT DEFINED CMAKE_TOOLCHAIN_FILE))
      install_windows_deps()
    endif()

    if(WIN32 AND (NOT DEFINED CMAKE_TOOLCHAIN_FILE))
      message(STATUS_FATAL "Windows dependency installation failed!")
    endif()
    if(WIN32)
      include(${CMAKE_TOOLCHAIN_FILE})
    endif()

    if(NOT protobuf_FOUND)
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
    set(PROTOBUF_PROTOC_EXECUTABLE ${Protobuf_PROTOC_EXECUTABLE})
  endif()
  message("PROTOBUF_PROTOC_EXECUTABLE=${PROTOBUF_PROTOC_EXECUTABLE}")

  include(cmake/opentelemetry-proto.cmake)
endif()

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

if(BUILD_TESTING)
  include(CTest)
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
    if(NOT GTEST_FOUND)
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

include_directories(api/include)
add_subdirectory(api)
include_directories(sdk/include)
include_directories(sdk)
add_subdirectory(sdk)
add_subdirectory(exporters)
if(WITH_EXAMPLES)
  add_subdirectory(examples)
endif()
add_subdirectory(ext)
