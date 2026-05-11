# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_executable(example_explicit_parent main.cc)
target_link_libraries(
  example_explicit_parent PRIVATE opentelemetry-cpp::trace
                                  opentelemetry-cpp::ostream_span_exporter)

if(BUILD_TESTING)
  add_test(NAME examples.explicit_parent
           COMMAND "$<TARGET_FILE:example_explicit_parent>")
endif()
