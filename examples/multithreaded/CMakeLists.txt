# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_executable(example_multithreaded main.cc)
target_link_libraries(
  example_multithreaded PRIVATE opentelemetry-cpp::trace
                                opentelemetry-cpp::ostream_span_exporter)

if(BUILD_TESTING)
  add_test(NAME examples.multithreaded
           COMMAND "$<TARGET_FILE:example_multithreaded>")
endif()
