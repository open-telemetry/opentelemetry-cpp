cmake_minimum_required(VERSION 3.1)

project(opentelemetry-cpp)

set(CMAKE_CXX_STANDARD 11)

include(CTest)

add_subdirectory(3rd_party)
add_subdirectory(api)
