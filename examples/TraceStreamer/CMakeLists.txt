#
# This reference example can be built as a standalone project
# including OpenTelemetry API as header-only. It provides a
# reference implementation of SDK that can stream spans to
# various 'stream' destinations (file, pipe, ETW, debugger).
#
cmake_minimum_required(VERSION 3.1.0)
project(TraceStreamer)

option(WITH_STL "Whether to use Standard Library for C++latest features" ON)

message ("CMAKE_CXX_COMPILER       = ${CMAKE_CXX_COMPILER}")
message ("CMAKE_CXX_COMPILER_ID    = ${CMAKE_CXX_COMPILER_ID}")
message ("CMAKE_COMPILER_IS_GNUCXX = ${CMAKE_COMPILER_IS_GNUCXX}")

set(CMAKE_OSX_SYSROOT /Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk)
set(CMAKE_OSX_SYSROOT ${CMAKE_OSX_SYSROOT} CACHE PATH "..." FORCE)

if(WITH_STL)
  set(CMAKE_CXX_STANDARD 20)
  add_definitions(-DHAVE_CPP_STDLIB -DHAVE_GSL)
  # Guidelines Support Library path
  set(GSL_DIR ../../third_party/ms-gsl)
  include_directories(${GSL_DIR}/include)
endif()

set(OPENTELEMETRY_API_DIR ../../api)

find_package (Threads)

# Open Telemetry API
include_directories( . ${OPENTELEMETRY_API_DIR}/include /usr/include)

# Link main.cpp to executable
add_executable(TraceStreamer main.cpp)
source_group(" " REGULAR_EXPRESSION "")

target_link_libraries(TraceStreamer ${CMAKE_THREAD_LIBS_INIT} ${PLATFORM_LIBS} ${CMAKE_DL_LIBS} ${GTEST_BOTH_LIBRARIES} ${CORE_RUNTIME_LIBS} benchmark::benchmark)
