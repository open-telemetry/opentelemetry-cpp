cmake_minimum_required(VERSION 3.1.0)
project(TraceStreamer)

option(WITH_STL "Whether to use Standard Library for C++latest features" OFF)

message ("${CMAKE_CXX_COMPILER}")
message ("${CMAKE_CXX_COMPILER_ID}")
message ("${CMAKE_COMPILER_IS_GNUCXX}")

set(CMAKE_OSX_SYSROOT ${CMAKE_OSX_SYSROOT} CACHE PATH "..." FORCE)

#set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -O0 -ggdb -gdwarf-2 -std=c1z")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -gdwarf-2 -std=c++1z")

#set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -O0 -ggdb -gdwarf-2 -std=c1z")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -gdwarf-2 -std=c++2a")

#set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -O0 -ggdb -gdwarf-2 -std=c1z")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -gdwarf-2 -std=c++17")

if(WITH_STL)
add_definitions(-DHAVE_STDLIB_CPP -DHAVE_GSL)
# Guidelines Support Library path
set(GSL_DIR ../../third_party/ms-gsl)
include_directories(${GSL_DIR}/include)
# TODO: use latest
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
endif()

set(OPENTELEMETRY_API_DIR ../../api)

find_package (Threads)

# Open Telemetry API
include_directories( . ${OPENTELEMETRY_API_DIR}/include /usr/include)

# Link main.cpp to executable
add_executable(TraceStreamer main.cpp)
source_group(" " REGULAR_EXPRESSION "")

target_link_libraries(TraceStreamer ${CMAKE_THREAD_LIBS_INIT} ${PLATFORM_LIBS} dl)
