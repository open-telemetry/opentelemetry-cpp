cmake_minimum_required(VERSION 3.1.0)
project(EventSampleCpp)

set(CMAKE_OSX_SYSROOT ${CMAKE_OSX_SYSROOT} CACHE PATH "..." FORCE)
#set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -O0 -ggdb -gdwarf-2 -std=c1z")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -gdwarf-2 -std=c++1z")

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -O0 -ggdb -gdwarf-2 -std=c1z")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -gdwarf-2 -std=c++2a")

#set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -O0 -ggdb -gdwarf-2 -std=c1z")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -gdwarf-2 -std=c++17")

#add_definitions(-DHAVE_STDLIB_CPP -DHAVE_GSL)

# MS Guidelines Support Library
set(GSL_DIR ../../third_party/ms-gsl)

set(OPENTELEMETRY_API_DIR ../../api)

find_package (Threads)

# Open Telemetry API
include_directories( . ${OPENTELEMETRY_API_DIR}/include /usr/include ${GSL_DIR}/include)

# Link main.cpp to executable
add_executable(EventSampleCpp main.cpp)
source_group(" " REGULAR_EXPRESSION "")

target_link_libraries(EventSampleCpp ${CMAKE_THREAD_LIBS_INIT} ${PLATFORM_LIBS} dl)
