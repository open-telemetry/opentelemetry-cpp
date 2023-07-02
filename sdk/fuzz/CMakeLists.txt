# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

enable_testing()
include(GoogleTest)

if(FUZZTEST_FUZZING_MODE)
  fuzztest_setup_fuzzing_flags()
endif()

foreach(testname atomic_unique_ptr_fuzz)

  add_executable(${testname} "${testname}.cc")
  link_fuzztest(${testname})
  target_link_libraries(${testname} PRIVATE opentelemetry_common
                                            opentelemetry_trace)
endforeach()
