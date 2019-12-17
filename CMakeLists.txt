cmake_minimum_required(VERSION 3.1)

project(opentelemetry-cpp)

set(CMAKE_CXX_STANDARD 11)

include(CTest)

find_package(Threads)

if (BUILD_TESTING)
  find_package(GTest REQUIRED)
  include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})
endif()

add_subdirectory(api)
