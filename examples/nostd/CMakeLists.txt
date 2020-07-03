cmake_minimum_required(VERSION 3.1.0)
project(nostd_usage)

message ("CMAKE_CXX_COMPILER       = ${CMAKE_CXX_COMPILER}")
message ("CMAKE_CXX_COMPILER_ID    = ${CMAKE_CXX_COMPILER_ID}")
message ("CMAKE_COMPILER_IS_GNUCXX = ${CMAKE_COMPILER_IS_GNUCXX}")

# Adjust this to use latest Mac OS X SDK
set(CMAKE_OSX_SYSROOT /Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk)
set(CMAKE_OSX_SYSROOT ${CMAKE_OSX_SYSROOT} CACHE PATH "..." FORCE)

set(CMAKE_CXX_STANDARD 20)

set(OPENTELEMETRY_API_DIR ../../api)

find_package (Threads)

# Open Telemetry API
include_directories( . ${OPENTELEMETRY_API_DIR}/include /usr/include)

add_executable(nostd_usage main.cpp)
source_group(" " REGULAR_EXPRESSION "")

target_link_libraries(nostd_usage ${CMAKE_THREAD_LIBS_INIT} ${PLATFORM_LIBS} ${CMAKE_DL_LIBS} ${GTEST_BOTH_LIBRARIES} benchmark::benchmark)
