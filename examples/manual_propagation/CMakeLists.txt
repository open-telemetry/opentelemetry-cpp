# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_executable(example_manual_propagation main.cc)
target_link_libraries(
  example_manual_propagation PRIVATE opentelemetry-cpp::trace
                        opentelemetry-cpp::ostream_span_exporter)

if(BUILD_TESTING)
  add_test(NAME examples.manual_propagation COMMAND "$<TARGET_FILE:example_manual_propagation>")
endif()
