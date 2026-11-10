# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Some 32-bit platforms (powerpc, mips, ...) need libatomic for 64-bit
# std::atomic operations. Detect that at configure time and expose
# OPENTELEMETRY_ATOMIC_LIB for targets that use those operations.
include_guard(GLOBAL)

set(OPENTELEMETRY_ATOMIC_LIB "")

function(otel_link_libatomic target)
  if(OPENTELEMETRY_ATOMIC_LIB)
    target_link_libraries(${target} PUBLIC ${OPENTELEMETRY_ATOMIC_LIB})
  endif()
endfunction()

if(MSVC)
  return()
endif()

include(CheckCXXSourceCompiles)
include(CMakePushCheckState)

set(_otel_atomic_src
    "
#include <atomic>
#include <cstdint>
std::atomic<uint64_t> x(0);
int main() {
  uint64_t i = x.load(std::memory_order_relaxed);
  x.store(i + 1, std::memory_order_relaxed);
  x.fetch_add(1, std::memory_order_relaxed);
  uint64_t expected = 0;
  x.compare_exchange_strong(expected, 1, std::memory_order_relaxed);
  return static_cast<int>(x.load(std::memory_order_relaxed) == 0);
}
")

cmake_push_check_state(RESET)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
  if(CMAKE_CXX_STANDARD)
    string(APPEND CMAKE_REQUIRED_FLAGS " -std=c++${CMAKE_CXX_STANDARD}")
  else()
    # The project accepts at least C++14 when the standard is unset.
    string(APPEND CMAKE_REQUIRED_FLAGS " -std=c++14")
  endif()
endif()
set(CMAKE_REQUIRED_QUIET TRUE)

check_cxx_source_compiles("${_otel_atomic_src}"
                          OPENTELEMETRY_HAVE_CXX_ATOMICS_WITHOUT_LIB)

if(NOT OPENTELEMETRY_HAVE_CXX_ATOMICS_WITHOUT_LIB)
  set(CMAKE_REQUIRED_LIBRARIES atomic)
  check_cxx_source_compiles("${_otel_atomic_src}"
                            OPENTELEMETRY_HAVE_CXX_ATOMICS_WITH_LIB)
  if(OPENTELEMETRY_HAVE_CXX_ATOMICS_WITH_LIB)
    set(OPENTELEMETRY_ATOMIC_LIB atomic)
    message(STATUS "Linking libatomic for 64-bit std::atomic support")
  else()
    message(
      WARNING
        "64-bit std::atomic failed to link both with and without libatomic. "
        "32-bit platforms may fail to link the SDK.")
  endif()
endif()

cmake_pop_check_state()
